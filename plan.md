# RC Car — Project Plan

> Original brainstorm notes are kept verbatim below; the worked plan follows.
> Mirrored in `ROADMAP.md`. Last updated 2026-10-11.

## Original notes



### (as first written)

I have an old nitro driven RC car which I have picked clean for parts. I only have the empty chassis and wheels.
My plan is to use a Nucleo F446RE STM32 dev board to test, try and build a working program for all the functions I would need.
The finished plan is to create a PCB that I can mount into the RC car which holds all logic for this project, there will also have to be a remote control box.
Perhaps I also want to create a program where I can connect my pc and send commands to perform certain tests and get a feedback in a terminal?

I do have four DC stepper motors, perhaps there is a way to have two motors as rear wheel drive? Not sure how the steering should work

Some of the biggest obsticles I have to overcome is getting the dc motors to run clean and be very responsive.
I then have to get a battery which I can use together with the motors, there is some SOC, SOH and current monitoring I would like to have etc

I then need to work out the steering of the car, either servo or stepper motor for this. 

Then there is a need for a remote control, needs logic in both ends for this 

All hardware points is a bit tricky for this project

---

# RC Car — Two-Month Bench Plan (no chassis required)

## Context

The project is to convert a stripped nitro RC car chassis to electric drive, controlled by a
Nucleo F446RE, eventually on a custom PCB with a separate remote-control box. The chassis is
unavailable for ~2 months (after Christmas 2026). The goal of this plan is to extract maximum
real progress from that window using only the Nucleo, the four 28BYJ-48/ULN2003 steppers, the
servos, and a PC.

The stated priority is **two motors, one per wheel, running nicely** as the first real target.

### The hardware reality that shapes everything

The 28BYJ-48 + ULN2003 cannot drive the car, and no amount of firmware will change that:

| | 28BYJ-48 available | 1/10 car needs (~5 km/h) |
|---|---|---|
| Output speed | ~15 RPM | ~300 RPM |
| Torque | ~29 mN·m | ~85 mN·m per driven wheel |
| Current | ~100 mA/phase, drawn **continuously** while holding | — |

The ULN2003 is an open-collector Darlington array with no current regulation, so as step rate
rises, coil inductance prevents current from reaching full value and torque collapses. ~15 RPM
is a physical ceiling.

**Consequence, and the core architectural decision of this plan:** treat the steppers as the
*bench drivetrain* and put the abstraction boundary at **commanded wheel velocity**. Everything
above that line — the mixer, ramping, the command protocol, telemetry, failsafe, battery
monitoring, the PC console — is written once and is independent of what spins the wheels. Only
a single backend module is thrown away when the real motors arrive. Done this way, the two
months produce roughly 80% of the final firmware, not a throwaway toy.

### The deliverable for the two months

A **bench rover**: two 28BYJ-48s with wheels and a caster on a scrap board or thick cardboard,
one steering servo on a dummy linkage, powered from a separate 5 V supply, driven live from a
Python console on the PC over the ST-Link virtual COM port, streaming telemetry back. It crawls
at ~5 cm/s. It is slow, and it exercises the entire final software stack end to end.

---

## Architecture

Five layers; the dashed line is the motor-swap boundary.

```
  PC console (Python)            Remote box (later; same protocol, different transport)
        |                                  |
        +---------------- framed packets ---+
                         |
  ┌──────────────────────┴──────────────────────┐
  │ L4  Link + failsafe: framing, CRC, timeout  │  loss of link -> coast
  ├─────────────────────────────────────────────┤
  │ L3  Vehicle control: throttle/steer mixer,  │  pure logic, host-testable
  │     trapezoidal ramping, limits, arming     │
  ├─────────────────────────────────────────────┤
  │ L2  Actuator abstraction                    │
  │     drive_set_wheel_velocity(side, v)       │  <-- THE BOUNDARY
  │     steer_set_angle(deg)                    │
  ├ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─┤
  │ L1  Backends: stepper (now) | H-bridge+enc  │  swapped after Christmas
  │     (later); servo PWM; INA226 battery      │
  ├─────────────────────────────────────────────┤
  │ L0  Board: clocks, GPIO, timers, UART, I2C  │
  └─────────────────────────────────────────────┘
```

Two design points that matter more than the rest:

1. **Step generation must be timer-ISR driven with a per-motor phase accumulator** — a fixed-rate
   ISR (e.g. 20 kHz) that adds each motor's velocity to an accumulator and advances that motor's
   coil phase on overflow. Never `delay()` loops. This is what lets two motors run at genuinely
   independent, smoothly-varying rates from one timer, and the same accumulator pattern reappears
   later for closed-loop DC control.

2. **L3 and L4 must have zero dependency on the HAL** so they compile with native gcc and are unit
   tested on the PC. The mixer, ramp generator, SOC estimator, and packet parser are all pure
   functions of state and input. This is where progress happens on days you don't want to touch
   hardware.

---

## Milestones

### Phase 0 — Toolchain and bench power (week 1)

- `openocd` and an ARM GDB (`arm-none-eabi-gdb` or `gdb-multiarch`) are **not installed**.
  `st-flash` can flash but gives no breakpoints or variable inspection; stepper timing bugs are
  painful to find by printf alone. Install both.
- **Separate 5 V supply for the motors.** Four 28BYJ-48s at ~200 mA each (two phases energized)
  is ~800 mA; the Nucleo's 5 V rail off USB cannot do this and brownouts will look like firmware
  bugs. Nucleo on USB, motors on their own supply, **grounds tied together at one point**.
- Prove the chain: blink, then `printf` over the ST-Link VCP at 115200.

**Done when:** a breakpoint can be hit in GDB and text arrives in a PC terminal.

### Phase 1 — The PC console, minimal (week 1–2)

Built first not because it is the goal but because it is the *instrument* for tuning everything
else. Keep it deliberately thin at this stage.

- Line-oriented ASCII command parser on the STM32 over UART; a periodic telemetry tick emitting
  one line of key=value at ~10 Hz.
- Python CLI on the PC: send commands, print telemetry, log to CSV for plotting.
- ASCII now, binary framed packets in Phase 5 — the parser lives behind an interface so the
  swap is contained.

**Done when:** `ping`, `telem on|off`, and `help` work from the PC, and telemetry logs to CSV.

### Phase 2 — Two wheels, done properly (week 2–4) ← the stated priority

This is the heart of the plan.

1. One motor: coil sequence tables (full-step and half-step), direction, and the 20 kHz
   ISR + phase accumulator. Verify steps/revolution empirically with a mark on the shaft —
   the 28BYJ-48 gear ratio is 63.68:1, not the 64:1 usually quoted, so counts drift if assumed.
2. Find the real speed ceiling: ramp step rate until it stalls or skips. That number is the
   velocity clamp.
3. **Trapezoidal velocity ramping** — accel/decel limits, so a command change is followed
   smoothly rather than instantly. This is the "clean and responsive" requirement, and the
   tension between those two words is the thing worth tuning here.
4. Second motor on independent rate from the same ISR. Confirm no interaction or jitter.
5. **The differential mixer:** `(throttle, steer) -> (v_left, v_right)` with clamping and
   normalization that preserves the turn ratio when saturating. Unit tested on the host. This
   module is used unchanged by the final car.
6. Console commands: `drive <throttle> <steer>`, `wheel <l|r> <vel>`, `stop`, `accel <limit>`.

**Done when:** the rover drives, steers by differential, and ramps smoothly — commanded from
the PC, with step rate and commanded/actual velocity visible in telemetry.

### Phase 3 — Steering servo (week 4–5)

Servo, not a stepper — a stepper has no absolute position at power-up and would need homing
hardware every boot, which is the wrong trade for steering.

- 50 Hz PWM, 1000–2000 µs, via a hardware timer channel.
- Endpoint limits, centre trim, and slew-rate limiting, all settable from the console and stored
  so the real linkage can be calibrated in minutes once the chassis exists.
- Mount on a dummy linkage, or just watch the horn.

**Done when:** `steer <deg>` moves the horn, endpoints clamp, and the servo cannot be commanded
to bind.

### Phase 4 — Power and battery monitoring (week 5–6)

- **INA226** (I²C, high-side bus voltage + current) is the right part. Order early.
- Coulomb counting for SOC, corrected against open-circuit voltage at rest. Host-unit-test the
  estimator by replaying a synthetic current trace.
- **Honest expectation on SOH:** state-of-health is capacity fade measured across many full
  cycles. It is not obtainable in two months, and not from an INA226 alone. Either accept that
  SOH arrives after months of logged cycles, or use a dedicated fuel-gauge IC (MAX17260,
  BQ34Z100) that ships with the modelling built in. Decide which, but don't expect SOH soon.
- Low-voltage cutoff and limp mode. Easy to get right now, unpleasant to debug in a moving car.

**Done when:** voltage, current, mAh consumed, and estimated SOC stream in telemetry, and the
motors cut out at a configured threshold.

### Phase 5 — Link protocol, failsafe, and ordering parts (week 6–8)

- Binary framed packets (start byte, length, payload, CRC) replacing the ASCII command path;
  PC stands in for the remote. The radio then becomes a transport swap, not a redesign, so the
  remote box stays off the critical path.
- **Failsafe is the most safety-relevant code in the project:** a watchdog on inbound packets;
  on timeout, ramp to zero and centre the steering. Test by yanking the USB cable mid-drive.
  Trivial to test now, dangerous to discover later.
- **Decide the real drivetrain and order before Christmas** so the chassis is assemblable the
  day it's free. Shipping over the holidays is slow.

**Done when:** the rover drives over the framed protocol and stops safely on cable-pull.

---

## Parts to order (lead time is the reason this is in the plan)

| Item | Why |
|---|---|
| Brushed DC motors + H-bridge (DRV8871 / BTS7960), **or** BLDC + hobby ESCs | The real drivetrain. One per rear wheel = electronic differential, no mechanical diff needed, and it maps 1:1 onto the bench mixer. |
| **Quadrature encoders** (or hall sensors) | The single biggest enabler of "clean and very responsive". Steppers are open-loop, so the bench rig cannot teach closed-loop control. The F446RE's timers do quadrature decoding in hardware. |
| INA226 breakout | Phase 4. |
| Radio pair — nRF24L01+ (cheap, SPI) or HC-12 (transparent serial, simplest) | Phase 5 transport. HC-12 if you want the protocol to work unchanged; nRF24 if you want lower latency. |
| 5 V bench supply or regulated module, ≥2 A | Phase 0. Needed immediately. |
| Separate steering servo for the bench | So the car's servo stays untouched. |

---

## What gets verified where

- **Host unit tests (native gcc, no hardware):** mixer saturation and turn-ratio preservation,
  ramp generator monotonicity and limit compliance, SOC estimator against replayed traces,
  packet parser against truncated/corrupt/fragmented input.
- **On the bench, via the PC console:** steps-per-rev measured against a shaft mark; max step
  rate before stall; ramp smoothness from CSV logs plotted on the PC; servo endpoints; INA226
  readings cross-checked against a multimeter; failsafe on cable pull.
- **After Christmas, on the car:** swap the L1 backend, recalibrate servo endpoints, retune
  accel limits against real mass and traction. If the boundary held, nothing above L2 changes.

## Risks

- **Scope creep into the remote box.** It is the most tempting and least necessary thing to build
  now. The PC is a better remote during development — it has a screen and logs.
- **Mistaking brownout for firmware bugs.** Phase 0's separate supply exists to prevent weeks of
  this.
- **Assuming 64:1.** The gearbox is 63.68:1; assumed counts drift over distance.
- **Expecting the bench rig to teach closed-loop control.** It cannot. That lesson waits for
  encoders, which is why they are on the order list rather than the nice-to-have list.

---

# Addendum A — Steering: servo, not stepper

Asked whether the spare 28BYJ-48s could steer instead of being wasted. For the car, no:

| | 28BYJ-48 | 1/10 steering needs |
|---|---|---|
| Torque | 29 mN·m (0.3 kg·cm) | 300–1000 mN·m (3–10 kg·cm) |
| Slew rate | ~90 °/s | 400–600 °/s |
| Position at power-up | unknown — needs homing | absolute |
| Backlash | 1–2° at output | straight into steering slop |
| Holding current | ~200 mA continuous | — |

~10–30x too weak, ~5x too slow. But the disqualifier is that it is **open-loop**: clip a kerb,
the stepper silently skips steps, and steering centre is now wrong with no way to detect it. On a
moving vehicle that is a runaway failure mode.

**Buy:** a standard-size metal-gear digital servo, 6–10 kg·cm (e.g. Savox SC-0251MG class). The
nitro chassis linkage was designed around a standard-size servo, so it should drop in. Power it
from its own 5–6 V BEC, never the Nucleo rail — servos pull 1–2 A at stall.

**Worthwhile use for one spare stepper:** on the bench, driving a dummy linkage with no tire load,
as a vehicle for writing **absolute position control + homing against an endstop switch**. That is
a genuinely different control mode from the velocity control in Phase 2, and the homing pattern
reappears any time an actuator lacks absolute feedback. Optional exercise; not the shipping path.

---

# Addendum B — The development board (do this in Phase 0/1)

This is the right thing to build now. It makes every later phase faster and it is pure gain —
nothing about it is wasted when the chassis arrives.

## Form factor: a desk board on a ribbon cable, NOT a shield

Resist making it a stacking shield. A shield covers the Nucleo's reset button, user button, user
LED and the ST-Link, and it puts every test point under a board. Instead:

- Perfboard / stripboard sitting flat on the desk.
- Connected to the Nucleo's **morpho** headers (2x38) by ribbon cable with IDC connectors —
  not the Arduino headers, whose pin subset you will outgrow.
- Everything socketed with female headers: the ULN2003 boards, INA226, nRF24, servo leads all
  plug in and can be swapped. The entire point is reconfigurability — you do not yet know what
  you need, which is also the reason **not** to spin a PCB yet. Spin the PCB in month two, once
  the perfboard has told you what you actually used.

## What goes on it

**Power — the most important section, and the one most often done badly**

- Screw terminals for external 5 V in, ≥2 A.
- **Separate power domains:** logic (Nucleo 3.3 V/5 V), motor 5 V, servo 5–6 V. Joined at exactly
  one **star ground point**. This is what prevents motor current spikes from resetting your MCU.
- A jumper selecting Nucleo-5V vs external-5V for the motor rail, so you cannot brown the board
  out by accident.
- Bulk capacitance: 1000 µF electrolytic on the motor rail, another on the servo rail, plus
  100 nF ceramics at each module socket.
- A power LED per rail. Trivial, and it answers "is this rail even up?" instantly.

**Test points** — underrated. A row of GND pins for scope ground clips, plus a labelled test point
on every timer output. You will be scoping step pulses and PWM constantly.

**Encoder interfaces** — 2 to 4 connectors of 4 pins each (5 V, GND, A, B), plus provision for an
index/Z channel. Must land on timer CH1+CH2 **of the same timer** to use the F446's hardware
quadrature decoding. On LQFP64, TIM1/2/3/4/5/8 support encoder mode — but TIM4's PD12/PD13 option
does not exist on this package, so use the PB6/PB7 mapping.

**A rotary encoder knob (EC11) + pushbutton** — for live tuning of accel limits and PID gains
without recompiling. Pairs extremely well with the PC console and is a large quality-of-life win
during the Phase 2 tuning work.

**Buttons** — 4 momentary + 1 latching E-stop. Pull-ups and RC debounce (10 kΩ + 100 nF).
**Gotcha that shapes the pin map:** STM32 EXTI lines are shared by *pin number* across all ports —
EXTI0 serves PA0/PB0/PC0, only one at a time. If you want each button on its own interrupt, every
button must sit on a **different pin number**.

**LEDs** — 6 to 8 with series resistors (~1 kΩ at 3.3 V). Put at least two on PWM-capable timer
channels: a dimmable LED is a perfect PWM test target, so you can verify duty-cycle and ramping
code with your eyes before risking a motor.

**Analog** — two potentiometers to ADC pins. These stand in for a throttle trigger and steering
wheel on the future remote, and let you test the battery voltage-divider path with a known input.

**Bus headers** — I2C (INA226, and an IMU later), SPI (nRF24: SCK/MISO/MOSI/CS/CE/IRQ), spare UART
(HC-12). Break these out even before you own the modules; the cost is a header.

## Pin budget — do this on paper BEFORE soldering

STM32 peripheral conflicts are unforgiving and resoldering a perfboard is miserable. LQFP64 gives
roughly 50 usable I/O after the reserved pins above:

| Block | Pins |
|---|---|
| 2 steppers x 4 coils | 8 |
| 2 quadrature encoders | 4 (timer CH1+CH2 pairs) |
| Servo PWM x2 | 2 |
| I2C1 | 2 |
| SPI1 + CE + IRQ | 6 |
| Spare UART | 2 |
| LEDs x6 | 6 |
| Buttons x5 | 5 (all different pin *numbers* — see EXTI note) |
| Pots x2 | 2 |
| Rotary knob + switch | 3 |
| **Total** | **~40 of ~50** |

Fits with headroom — but note that **four** steppers plus everything else does not (16+32 = ~49,
at the ragged edge). Two steppers is the plan, so this is fine; just don't plan for four.

**Escape hatch if pins get tight:** an MCP23017 I2C GPIO expander gives 16 pins for 2. Fine for
LEDs and buttons; useless for steppers or encoders, where the I2C round-trip latency matters.

**Suggested starting allocation** (verify every alternate-function number against the F446RE
datasheet AF tables before you solder — these are the conventional mappings, not gospel):

- Encoder 1 -> TIM3 CH1/CH2 on **PA6/PA7**
- Encoder 2 -> TIM4 CH1/CH2 on **PB6/PB7**
- Encoders 3/4 (spare) -> TIM8 on **PC6/PC7**, TIM1 on **PA8/PA9**
- Servos -> any spare TIM2/TIM5 channel at 50 Hz
- Steppers -> plain GPIO, any port, grouped on one port per motor so a single BSRR write sets all
  four coils atomically. Worth doing: it makes the ISR cheaper and the phase change glitch-free.
- I2C1 -> **PB8/PB9** (keeps PB6/PB7 free for the encoder above)

Keep the map in a text file in the repo and update it as you wire. It is the single document you
will consult most often.

## Done when

External rails come up with their LEDs lit, a button press toggles an LED through an interrupt, a
hand-turned encoder increments a hardware timer count visible in telemetry, and the knob changes a
live tuning value — all before a single motor is connected.
