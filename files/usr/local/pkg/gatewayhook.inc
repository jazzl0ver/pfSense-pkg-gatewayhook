<?php
/*
 * gatewayhook.inc
 *
 * Gateway up/down hook. Runs /usr/local/etc/rc.d/rc.gateway_alarm_custom upon gateway up/down event
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* include all configuration functions */
require_once("functions.inc");
require_once("service-utils.inc");

define('GATEWAY_ALARM_CUSTOM_SCRIPT', '/usr/local/etc/rc.d/rc.gateway_alarm_custom');

function gatewayhook_install() {
    if (!file_exists(GATEWAY_ALARM_CUSTOM_SCRIPT)) {
	file_put_contents(GATEWAY_ALARM_CUSTOM_SCRIPT, 
	    '#!/bin/sh' . "\n\n" .
	    '# don\'t run the hook script on CARP slave' . "\n\n" .
	    'ifconfig | grep "carp: " | grep -q MASTER || exit 0' . "\n\n" .
	    '# put what needs to be done before exit line' . "\n\n" .
	    'exit 0' . "\n"
	);
	chmod(GATEWAY_ALARM_CUSTOM_SCRIPT, 0755);
    }
}

function gatewayhook_plugin_gateway($pluginparams) {
    $type = $pluginparams['type'];
    $event = $pluginparams['event'];
    $interface = $pluginparams['interface'];
    $gatewayhooklock = lock("gatewayhook", LOCK_EX);
    syslog(LOG_NOTICE, "gatewayhook: " . GATEWAY_ALARM_CUSTOM_SCRIPT . " script started");
    mwexec("type={$type} event={$event} interface={$interface} " . GATEWAY_ALARM_CUSTOM_SCRIPT);
    unlock($gatewayhooklock);
    return 0;
}

gatewayhook_install();
