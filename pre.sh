#!/bin/sh
eeprom fcode-debug\?=true
eeprom nvramrc='" /iommu/sbus/cgsix@2" select-dev
