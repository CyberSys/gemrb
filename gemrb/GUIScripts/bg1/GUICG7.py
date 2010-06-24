# -*-python-*-
# GemRB - Infinity Engine Emulator
# Copyright (C) 2003-2004 The GemRB Project
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# character generation, mage spells (GUICG7)

import GemRB
from GUIDefines import *
import LUSpellSelection

def OnLoad():
	KitTable = GemRB.LoadTable("magesch")
	Slot = GemRB.GetVar ("Slot")
	Class = GemRB.GetPlayerStat (Slot, IE_CLASS)
	TableName = GUICommon.ClassSkillsTable.GetValue(Class, 2)

	# make sure we have a correct table
	if Class == 19:
		# sorcerer's need their known not max table
		TableName = "SPLSRCKN"

	# get our kit index
	KitIndex = GUICommon.GetKitIndex (Slot)
	if KitIndex:
		KitValue = KitTable.GetValue(KitIndex - 21, 3)

		# bards have kits too
		if KitValue == -1:
			KitValue = 0x4000 # we only need it for the spells, so this is ok
	else:
		KitValue = 0x4000

	# open up the spell selection window
	# remember, it is pc, table, level, diff, kit, chargen
	IsMulti = GUICommon.IsMultiClassed (Slot, 1)
	Level = GemRB.GetPlayerStat (Slot, IE_LEVEL)
	if IsMulti[0]>1:
		for i in range (2, IsMulti[0]+1):
			if GUICommon.ClassSkillsTable.GetValue (IsMulti[i], 2, 0) != "*":
				Level = GemRB.GetPlayerStat (Slot, IE_LEVEL2+i-1)
			break
	GUICommon.SetupSpellLevels(Slot, TableName, IE_SPELL_TYPE_WIZARD, 1)
	LUSpellSelection.OpenSpellsWindow (Slot, TableName, Level, Level, KitValue, 1,False)

	return
