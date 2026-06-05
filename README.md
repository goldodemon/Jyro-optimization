# Jyro Optimization

Jyro Optimization is a sleek, minimal C++ PC optimization prototype inspired by tools like Hone.gg. It uses blue and light-blue terminal styling while keeping the terminal background untouched, which lets transparent terminals stay clean.

> Jyro Optimization is not affiliated with Hone.gg or Gumroad.

## Product concept

- **Lite:** 15 free optimizations and 3 sampled premium features.
- **Premium:** Unlimited optimizations and unlimited premium exclusives after entering a Gumroad license key.
- **Normal optimization target:** 35 normal optimizations.
- **Premium exclusive target:** 25 premium exclusives.
- **Core features:** Game Mode, Boost-Ups, Discord Support, Power Mode, Game Optimizations, Ad-Free Experience, and theme customization.
- **Safety:** Windows builds automatically request administrator privileges and must create a restore point before optimization workflows continue.

## Gumroad licensing

The prototype includes the supplied product ID:

```text
frOFk3Qp2UjzrYLSnoEVOQ==
```

Windows builds post the product ID and user-entered key to Gumroad's license verification endpoint. Non-Windows builds keep a deterministic format-only check so the prototype can be built and demonstrated without platform-specific networking dependencies. Do not ship private API credentials inside the desktop app.

## Build

```bash
cmake -S . -B build
cmake --build build
```

Run the app:

```bash
./build/jyro-optimization
```

On Windows, run the generated executable from an account that can approve the administrator prompt.
