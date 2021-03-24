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
#ifndef COMMON_H
#define COMMON_H

#define APP_NAME							"kylin-scanner"
#define CN_TRANSLATION_FILE_PATH			"/usr/share/kylin-scanner/translations/kylin-scanner."

#define LOG_PATH 							"/tmp/scanner/"
#define LOGFILE_PATH						"/tmp/scanner/kylin-scanner.log"
#define EXIST_USERNAME_AND_PID_PATH 		"/tmp/scanner/user.pid"

#define BASH_TYPE 							"/bin/bash"

#define SUCCESS								0
#define FAIL								-1

// Define colors
#define GREEN_COLOR							"\033[32m"
#define BLUE_COLOR							"\033[34m"
#define RED_COLOR 							"\033[31m"
#define YELLOW_COLOR						"\033[33m"
#define NONE_COLOR 							"\033[0m"

#endif // COMMON_H
