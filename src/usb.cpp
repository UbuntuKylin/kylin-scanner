/*
* Copyright (C) 2020, KylinSoft Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/

#include "usb.h"

int init_hotplug_sock()
{
    const int buffersize = 1024;
    int ret;
    struct sockaddr_nl snl;
    bzero(&snl, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    snl.nl_groups = 1;
    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (s == -1) {
        perror("socket");
        return -1;
    }
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));
    ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
    if (ret < 0) {
        perror("bind");
        close(s);
        return -1;
    }
    return s;
}

