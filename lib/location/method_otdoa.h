/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef METHOD_OTDOA_H
#define METHOD_OTDOA_H

int method_otdoa_init(void);
int method_otdoa_location_get(const struct location_request_info *request);
int method_otdoa_cancel(void);
int method_otdoa_timeout(void);

#endif /* METHOD_OTDOA_H */
