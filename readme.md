![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Harmony 3 Peripheral Library Application Examples for SAMA7D65 Family

MPLAB® Harmony 3 is an extension of the MPLAB® ecosystem for creating embedded firmware solutions for Microchip 32-bit SAM and PIC® microcontroller and microprocessor devices.  Refer to the following links for more information.

- [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit)
- [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus)
- [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
- [Microchip MPLAB® Harmony](https://www.microchip.com/mplab/mplab-harmony)
- [Microchip MPLAB® Harmony Pages](https://microchip-mplab-harmony.github.io/)

This repository contains the MPLAB® Harmony 3 peripheral library application examples for SAMA7D65 family:

- [Release Notes](release_notes.md)
- [MPLAB® Harmony License](Microchip_SLA001.md)

To clone or download these applications from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sama7d65) and then click **Clone**<br /> button to clone this repository or download as zip file. This content can also be<br /> downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki).

## Contents Summary

|Folder|Description|
|:-----|:----------|
|apps|Contains peripheral library example<br /> applications|
|docs|Contains documentation in html<br /> format for offline viewing \(to be used only after cloning this<br /> repository onto a local machine\). Use [github pages](https://microchip-mplab-harmony.github.io/) of this repository for<br /> viewing it online|

## Code Examples

The following applications are provided to demonstrate the typical or interesting<br /> usage models of one or more peripheral libraries.

| Name | Description |
| ---- | ----------- |
| [ACC interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-C08B1311-7634-4864-8941-559EFBE19612.html) | This example application shows how to use an analog comparator controller (ACC) generate an interrupt on a compare event.  |
| [ADC automatic window comparison](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-EFF21550-01F9-45A6-8F87-385532E00D30.html) | This example application shows how to sample an analog input in polled mode and send the converted data to console using automatic window comparison of converted values |
| [ADC Polling](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-3536B2EE-63DC-4A4A-8F2B-C5C7140F0BCB.html) | This example application shows how to sample an analog input in polled mode and send the converted data to console |
| [ADC user sequence](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-311D527F-B36E-40F9-B8D9-B61FDF2C0945.html) | This example application shows how to sample three analog inputs using the user sequencer in external trigger mode (TC peripheral) and send the converted data to the console |
| [Clock configuration](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-3AE1E594-3DB1-4020-A4DC-57CA226621DD.html) | This example application shows how to configure the clock system to run the device at maximum frequency. It also outputs a prescaled clock signal on a GPIO pin for measurement and verification |
| [DWDT timeout](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-1FF2B42F-8C71-40AA-83FB-01ACC73537F9.html) | This example application shows how to generate a Dual Watchdog timer reset by simulating a deadlock |
| [FLEXCOM SPI EEPROM read write](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-0A23DD77-9776-43D7-BE8A-CCD9B872DBF0.html) | This example application shows how to use the flexcom module in SPI mode |
| [FLEXCOM TWI (I2C) EEPROM](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-9027F1BA-426E-4C29-A594-CB8C31556A45.html) | This example application shows how to use the flexcom module in TWI mode |
| [FLEXCOM USART interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-99F9B63A-982D-4876-B899-8BAE0901A790.html) | This example application shows how to use the flexcom module in USART mode |
| [FLEXCOM USART flow control](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-A5C57B0D-3808-43CE-A549-BA2FB5819E08.html) | This example application shows how to use the flexcom module in USART flow control mode |
| [FLEXCOM USART ring buffer](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-6957E6F0-7343-42E5-A697-EA1307222AB1.html) | This example application shows how to use the FLEXCOM peripheral in USART ring buffer mode |
| [Generic Timer periodic interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-C504C722-4D75-43FB-87D6-1A2A0D9DFC64.html) | This application shows how to use Generic timer counter in interrupt mode |
| [MCAN FD blocking](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-9A9DCEF9-8A29-4DC1-BDF3-1945FBD7EEA2.html) | This example shows how to use the MCAN module to transmit and receive CAN FD messages in polling mode |
| [MCAN FD interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-9E86542E-9E3A-4510-BAD3-01F53D71F0D1.html) | This example shows how to use the MCAN module to transmit and receive CAN FD messages in interrupt mode |
| [OTPC read write (emulation)](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-BBFF31AF-A882-4B92-906B-2CF4EF0292D2.html) | This example application shows how to use the OTPC Peripheral library to perform OTP operations |
| [PIO interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-9F0A2526-E188-4342-A261-682012CD054F.html) | This example application shows how to generate GPIO interrupt on switch press and release, and indicate the switch status using the LED |
| [PIT64B periodic interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-25912ACA-6FD4-4AD2-90FF-1AAE46F09A4A.html) | This application shows how to use PIT64B counter in interrupt mode |
| [PWM Generation](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-7A783AEC-4E6E-400E-8AE6-1B01C2A6D316.html) | This example shows how to use the PWM peripheral to generate 3-phase PWM signals with dead time |
| [QSPI flash read write](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-622E6063-0C0B-47FF-BD57-6D895EE73C73.html) | This example shows how to use the QSPI Peripheral library to perform erase, write and read operation with the QSPI Serial Flash memory in Quad IO mode |
| [QSPI flash read write in SPI mode](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-8E4A3BC9-291F-4306-9A34-606CA7DFD2D5.html) | This example shows how to use the QSPI Peripheral library to perform erase, write and read operation with the QSPI Serial Flash memory in SPI mode |
| [RSTC reset cause](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-3E6FAF69-D926-4BF0-B564-488B376854AF.html) | This example shows how to use the RSTC peripheral to indicate the cause of the device reset |
| [RTC alarm interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-02A14321-B3A3-4FDB-A2ED-35B11894AA64.html) | This example application shows how to use the RTC to configure the time and generate the alarm |
| [RTT alarm interrupt](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-7E49AF24-E123-424A-A049-67B5ED7B51A3.html) | This example application shows how to use the RTT to generate alarm interrupt |
| [SHDWC wakeup](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-9E92EABB-1986-4C90-8ADA-8C85BB01C710.html) | This example application shows how to shutdown and wakeup the device using SHDWC peripheral |
| [TC capture mode](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-5F620C43-478E-4F1B-A568-AACF6B91752A.html) | This example application shows how to use the TC module in capture mode to measure duty cycle and frequency of an external input |
| [TC compare mode](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-F65BFBDE-DC71-475A-9F69-B2A6E561A16A.html) | This example application shows how to use the TC module in compare mode to generate an active low, active high, and toggle output on compare match |
| [TC timer mode](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-0EDB2604-278A-4148-B019-C5A21C09BF1A.html) | This example application shows how to use the TC module in timer mode to generate periodic interrupt |
| [TRNG random number](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-F3D335ED-BC7F-4D61-889D-BDB4D4DCA393.html) | This example application shows how to use the TRNG Peripheral library to generate and read a random number |
| [XDMAC memory transfer](https://microchip-mplab-harmony.github.io/csp_apps_sama7d65/GUID-CDD29EE9-29A4-4359-BD99-EDFD308E5FDA.html) | This example application shows how to use the XDMAC peripheral to do a memory to memory transfer and illustrates the usage of burst size to reduce the transfer time |


____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sama7d65/blob/master/mplab_harmony_license.md)
[![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg)](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sama7d65/releases/latest)
[![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg)](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sama7d65/releases/latest)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg)](https://github.com/Microchip-MPLAB-Harmony/csp_apps_sama7d65/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg)]()

____

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/csp_apps_sama7d65.svg?style=social)]()


