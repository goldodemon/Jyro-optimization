# Jyro Optimization

Jyro Optimization is a sleek, minimal C++ Windows desktop optimizer inspired by the clean presentation style of Hone.gg. The interface uses a blue and light-blue mixed theme, a translucent layered window, and a custom-drawn Jyro logo.

## Editions

| Edition | Access |
| --- | --- |
| Lite | 15 free optimizations plus 3 premium previews |
| Premium | Unlimited optimizations, all 40 normal optimizations, and 30 premium exclusives |

## Included feature areas

- Game Mode and game scheduling tweaks
- Boost-Ups, Power Mode, and performance power profiles
- Discord Support hooks and diagnostics
- Theme customization with premium visual presets
- Ad-free interface
- Gumroad license verification for Premium access
- Automatic administrator relaunch on Windows
- Mandatory restore point creation before optimization batches

## Gumroad setup

The Windows app posts license verification requests to Gumroad's license API with:

- `product_id=frOFk3Qp2UjzrYLSnoEVOQ==`
- `license_key=<user-entered key>`
- `increment_uses_count=false`

Premium controls remain disabled until Gumroad returns a successful, non-refunded, non-disputed purchase response.

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

On Windows, the `JyroOptimization` target builds the native Win32 UI. On non-Windows platforms, the target prints the catalog counts so CI can still validate the C++ project structure.
