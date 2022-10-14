# NL-840 Nixie clock project

This repository contains resources to craft an NL-840 4-digits Nixie precise Nixie clock.

This work is greatly inspired from the project of [Davide Bucci]<http://davbucci.chez-alice.fr/index.php?argument=elettronica/orolnixie21/orolnixie21.inc> with some software and hardware upgrade.

It aims to have a precision of less than 10PPM (half minute shift on a month period) without relying on interrupt signals. It's using a 16F628A pic.

## History

### Version 1 (without external oscillator)

The first version is counting clock cycles to measure minutes. However, as the multiplexing requires some bit operations, the amount of cycles vary with the displayed time.
This approach leads to a precision of 180PPM.

### Version 2 (with external oscillator)

By using an external RTC module to measure the time, empirical clock cycles counting fix is no longer required.

## Ressources

* Source codes of version 1 and 2 in the [src]</src> folder
* Electronic schematics (@todo)

## Shopping list

* RTC module: <https://www.amazon.fr/gp/product/B07XTBDNZ5/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1>
* Cheap Microcontroller programming board to burn the microcontroller if you're on a budget: <https://www.banggood.com/fr/PIC-Microcontroller-USB-Automatic-Programming-Programmer-MCU-Microcore-Burner-USB-Downloader-K150-ICSP-Cable-p-1557736.html?rmmds=myorder&cur_warehouse=CN>
* PIC 16F628A
* Metal pieces known as electronic components (@todo)

## Safety

Handling high voltages plugged to your electricity mains is dangerous. Be careful and proceed at your own risks.
