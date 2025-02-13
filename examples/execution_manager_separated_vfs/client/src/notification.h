/*
 * © 2025 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#ifndef EXECUTION_MANAGER_NOTIFICATION_H
#define EXECUTION_MANAGER_NOTIFICATION_H

#include <rtl_cpp/retcode.h>

kos::rtl::Result PublishService();
kos::rtl::Result WaitForNotification();

#endif // EXECUTION_MANAGER_NOTIFICATION_H
