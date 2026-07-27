# Extra examples for KasperskyOS Community Edition

This repository contains C/C++ examples of KasperskyOS-based solutions. These examples are designed
to help developers understand and implement different KasperskyOS features in their projects.
For additional details on KasperskyOS, including its limitations and known issues, please refer to the
[KasperskyOS Community Edition Online Help](https://click.kaspersky.com/?hl=en-us&link=online_help&pid=kos&version=1.4&customization=KCE&helpid=community_edition).

## System components and IPC

* [Using IPC return and error handling](examples/errors_via_ipc/README.md)
* [Using the ExecutionManager component](examples/execution_manager_separated_vfs/README.md)
* [Using the nkppmeta compiler](examples/nkpp/README.md)
* [Using the PackageManager component](examples/package_manager_utilities/README.md)
* [Using IPC with a sequence parameter type](examples/sequence/README.md)

## Hardware and drivers

* [Using MBIM network driver for internet access](examples/cellular/mbim_ntpd/README.md)
* [Using MBIM network driver for sending SMS](examples/cellular/mbim_sms/README.md)
* [Using USB UVC camera to capture images](examples/uvc_camera_stills/README.md)

## Network and connectivity

* [Using the wpa_supplicant client library](examples/wifi/wpa_supplicant_client/README.md)
* [Using the iperf application in combination with the wpa_supplicant client library](examples/wifi/iperf_wifi/README.md)

## External library integrations

* [Using the KasperskyOS-adapted CPP-JWT library to generate a token](examples/jwt/README.md)
* [Using the KasperskyOS-adapted Abseil library to integrate foundational C++ components](examples/abseil/README.md)
* [Using the KasperskyOS-adapted RabbitMQ-C library to implement AMQP consumer application](examples/rabbitmq/amqp_consumer/README.md)
* [Using the KasperskyOS-adapted RabbitMQ-C library to implement AMQP publisher application](examples/rabbitmq/amqp_publisher/README.md)
* [Using the KasperskyOS-adapted gRPC library to implement cross-platform client-server communication](examples/grpc/README.md)
* [Using the KasperskyOS-adapted BoringSSL library to implement cryptographic operations](examples/boringssl/README.md)
* [Using the KasperskyOS-adapted c-ares library to implement asynchronous DNS resolution](examples/c-ares/README.md)
* [Using the KasperskyOS-adapted protobuf library to implement structured data exchange via IPC](examples/protobuf/README.md)
* [Using the KasperskyOS-adapted CPython to run a simple script](examples/cpython/hello/README.md)
* [Using the KasperskyOS-adapted CPython to run third-party Python packages](examples/cpython/numpy/README.md)

# Trademarks

Registered trademarks and endpoint marks are the property of their respective owners.

Android, Google and PROTOBUF are trademarks of Google LLC.

Apache is either a registered trademark or a trademark of the Apache Software Foundation in the
United States and/or other countries.

Arm is a registered trademark of Arm Limited (or its subsidiaries) in the US and/or elsewhere.

Apple, AppleTalk and Mac are trademarks of Apple Inc.

Cisco, Jabber are registered trademarks or trademarks of Cisco Systems, Inc. and/or its affiliates
in the United States and certain other countries.

ClearCase, Lotus are registered trademarks of International Business Machines Corporation,
registered in many jurisdictions worldwide.

Debian is a registered trademark of Software in the Public Interest, Inc.

DICOM® is the registered trademark of the National Electrical Manufacturers Association for its
Standards publications relating to digital communications of medical information.

Docker and the Docker logo are trademarks or registered trademarks of Docker, Inc. in the United
States and/or other countries. Docker, Inc. and other parties may also have trademark rights in
other terms used herein.

Dropbox is a trademark of Dropbox, Inc.

F5 is a trademark of F5 Networks, Inc. in the U.S. and in certain other countries.

GITHUB is a trademark of GitHub, Inc., registered in the United States and other countries.

GRPC is a registered trademark of The Linux Foundation in the United States and other countries.

Java, JavaScript are registered trademarks of Oracle and/or its affiliates.

Linux is the registered trademark of Linus Torvalds in the U.S. and other countries.

Microsoft, MSN and SQL Server are trademarks of the Microsoft group of companies.

NetBackup, Veritas are trademarks or registered trademarks of Symantec Corporation or its
affiliates in the U.S. and other countries.

Node.js is a trademark of Joyent, Inc.

OpenSSL is a trademark owned by the OpenSSL Software Foundation.

BoringSSL adaptation for KasperskyOS is not affiliated with OpenSSL.

OpenVPN is a registered trademark of OpenVPN, Inc.

Python is a trademark or registered trademark of the Python Software Foundation.

Puppet is a trademark or registered trademark of Puppet, Inc.

QT is a trademark or registered trademark of The Qt Company Ltd.

RabbitMQ is a trademark of Broadcom, Inc. in the U.S. and other countries.

Radmin is a registered trademark of Famatech.

Raspberry Pi is a trademark of the Raspberry Pi Foundation.

UNIX is a registered trademark in the United States and other countries, licensed exclusively
through X/Open Company Limited.

Zabbix is a registered trademark of Zabbix SIA.

# Contributing

Only KasperskyOS-specific changes can be approved. See [CONTRIBUTING.md](CONTRIBUTING.md) for
detailed instructions on code contribution.

# Licensing

This project is licensed under the terms of the MIT license. See [LICENSE](LICENSE) for more
information.

© 2026 AO Kaspersky Lab
