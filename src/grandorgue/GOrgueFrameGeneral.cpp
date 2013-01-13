/*
 * GrandOrgue - free pipe organ simulator
 *
 * Copyright 2006 Milan Digital Audio LLC
 * Copyright 2009-2012 GrandOrgue contributors (see AUTHORS)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "GOrgueFrameGeneral.h"

#include "GOrgueConfigReader.h"
#include "GOrgueConfigWriter.h"
#include "GOrgueDivisional.h"
#include "GOrgueGeneral.h"
#include "GOrgueManual.h"
#include "GOrgueSetter.h"
#include "GrandOrgueFile.h"

GOrgueFrameGeneral::GOrgueFrameGeneral(GOrgueCombinationDefinition& general_template, GrandOrgueFile* organfile):
	GOrgueCombination(general_template, organfile),
	m_organfile(organfile),
	m_group()
{
}

void GOrgueFrameGeneral::Load(GOrgueConfigReader& cfg, wxString group)
{
	m_group = group;

	m_Protected = cfg.ReadBoolean(ODFSetting, group, wxT("Protected"), false, false);
}

void GOrgueFrameGeneral::LoadCombination(GOrgueConfigReader& cfg)
{
	wxString buffer;
	unsigned NumberOfStops = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfStops"), 0, 999);
	unsigned NumberOfCouplers = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfCouplers"), 0, 999);
	unsigned NumberOfTremulants = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfTremulants"), 0, 999);
	unsigned NumberOfDivisionalCouplers = cfg.ReadInteger(UserSetting, m_group, wxT("NumberOfDivisionalCouplers"), 0, 999, m_organfile->GeneralsStoreDivisionalCouplers());

	int pos;
	UpdateState();
	for(unsigned i = 0; i < m_State.size(); i++)
		m_State[i] = -1;

	for (unsigned i = 0; i < NumberOfStops; i++)
	{
		buffer.Printf(wxT("StopManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(UserSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		buffer.Printf(wxT("StopNumber%03d"), i + 1);
		int s = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetManual(m)->GetStopCount(), m_organfile->GetManual(m)->GetStopCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_STOP, m, abs(s));
		if (pos >= 0)
			m_State[pos] = (s > 0) ? 1 : 0;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfCouplers; i++)
	{
		buffer.Printf(wxT("CouplerManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(UserSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		buffer.Printf(wxT("CouplerNumber%03d"), i + 1);
		int s = cfg.ReadInteger(UserSetting, m_group, buffer, -999, 999);
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_COUPLER, m, abs(s));
		if (pos >= 0)
			m_State[pos] = (s > 0) ? 1 : 0;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfTremulants; i++)
	{
		buffer.Printf(wxT("TremulantNumber%03d"), i + 1);
		int s = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetTremulantCount(), m_organfile->GetTremulantCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_TREMULANT, -1, abs(s));
		if (pos >= 0)
			m_State[pos] = (s > 0) ? 1 : 0;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfDivisionalCouplers; i++)
	{
		buffer.Printf(wxT("DivisionalCouplerNumber%03d"), i + 1);
		int s = cfg.ReadInteger(UserSetting, m_group, buffer, -m_organfile->GetDivisionalCouplerCount(), m_organfile->GetDivisionalCouplerCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_DIVISIONALCOUPLER, -1, abs(s));
		if (pos >= 0)
			m_State[pos] = (s > 0) ? 1 : 0;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	/* skip ODF settings */
	NumberOfStops = cfg.ReadInteger(ODFSetting, m_group, wxT("NumberOfStops"), 0, 999);
	NumberOfCouplers = cfg.ReadInteger(ODFSetting, m_group, wxT("NumberOfCouplers"), 0, 999);
	NumberOfTremulants = cfg.ReadInteger(ODFSetting, m_group, wxT("NumberOfTremulants"), 0, 999);
	NumberOfDivisionalCouplers = cfg.ReadInteger(ODFSetting, m_group, wxT("NumberOfDivisionalCouplers"), 0, 999, m_organfile->GeneralsStoreDivisionalCouplers());

	for (unsigned i = 0; i < NumberOfStops; i++)
	{
		buffer.Printf(wxT("StopManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(ODFSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		buffer.Printf(wxT("StopNumber%03d"), i + 1);
		int s = cfg.ReadInteger(ODFSetting, m_group, buffer, -m_organfile->GetManual(m)->GetStopCount(), m_organfile->GetManual(m)->GetStopCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_STOP, m, abs(s));
		if (pos >= 0)
			;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfCouplers; i++)
	{
		buffer.Printf(wxT("CouplerManual%03d"), i + 1);
		unsigned m = cfg.ReadInteger(ODFSetting, m_group, buffer, m_organfile->GetFirstManualIndex(), m_organfile->GetManualAndPedalCount());
		buffer.Printf(wxT("CouplerNumber%03d"), i + 1);
		int s = cfg.ReadInteger(ODFSetting, m_group, buffer, -999, 999);
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_COUPLER, m, abs(s));
		if (pos >= 0)
			;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfTremulants; i++)
	{
		buffer.Printf(wxT("TremulantNumber%03d"), i + 1);
		int s = cfg.ReadInteger(ODFSetting, m_group, buffer, -m_organfile->GetTremulantCount(), m_organfile->GetTremulantCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_TREMULANT, -1, abs(s));
		if (pos >= 0)
			;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

	for (unsigned i = 0; i < NumberOfDivisionalCouplers; i++)
	{
		buffer.Printf(wxT("DivisionalCouplerNumber%03d"), i + 1);
		int s = cfg.ReadInteger(ODFSetting, m_group, buffer, -m_organfile->GetDivisionalCouplerCount(), m_organfile->GetDivisionalCouplerCount());
		pos = m_Template.findEntry(GOrgueCombinationDefinition::COMBINATION_DIVISIONALCOUPLER, -1, abs(s));
		if (pos >= 0)
			;
		else
			wxLogError(_("Invalid combination entry %s in %s"), buffer.c_str(), m_group.c_str());
	}

}

void GOrgueFrameGeneral::Push()
{
	bool used = GOrgueCombination::PushLocal();

	for (unsigned k = 0; k < m_organfile->GetGeneralCount(); k++)
	{
		GOrgueGeneral* general = m_organfile->GetGeneral(k);
		general->Display(&general->GetGeneral() == this && used);
	}

	for (unsigned j = m_organfile->GetFirstManualIndex(); j <= m_organfile->GetManualAndPedalCount(); j++)
	{
		for (unsigned k = 0; k < m_organfile->GetManual(j)->GetDivisionalCount(); k++)
			m_organfile->GetManual(j)->GetDivisional(k)->Display(false);
	}

	m_organfile->GetSetter()->ResetDisplay();
}

void GOrgueFrameGeneral::Save(GOrgueConfigWriter& cfg)
{
	UpdateState();
	const std::vector<GOrgueCombinationDefinition::CombinationSlot>& elements = m_Template.GetCombinationElements();

	wxString buffer;
	unsigned stop_count = 0;
	unsigned coupler_count = 0;
	unsigned tremulant_count = 0;
	unsigned divisional_coupler_count = 0;

	for(unsigned i = 0; i < elements.size(); i++)
	{
		if (m_State[i] == -1)
			continue;
		int value = m_State[i] == 1 ? elements[i].index : -elements[i].index;
		switch(elements[i].type)
		{
		case GOrgueCombinationDefinition::COMBINATION_STOP:
			stop_count++;
			buffer.Printf(wxT("StopManual%03d"), stop_count);
			cfg.Write(m_group, buffer, elements[i].manual, true, true);
			buffer.Printf(wxT("StopNumber%03d"), stop_count);
			cfg.Write(m_group, buffer, value, true);
			break;

		case GOrgueCombinationDefinition::COMBINATION_COUPLER:
			coupler_count++;
			buffer.Printf(wxT("CouplerManual%03d"), coupler_count);
			cfg.Write(m_group, buffer, elements[i].manual, true, true);
			buffer.Printf(wxT("CouplerNumber%03d"), coupler_count);
			cfg.Write(m_group, buffer, value, true);
			break;

		case GOrgueCombinationDefinition::COMBINATION_TREMULANT:
			tremulant_count++;
			buffer.Printf(wxT("TremulantNumber%03d"), tremulant_count);
			cfg.Write(m_group, buffer, value, true);
			break;

		case GOrgueCombinationDefinition::COMBINATION_DIVISIONALCOUPLER:
			divisional_coupler_count++;
			buffer.Printf(wxT("DivisionalCouplerNumber%03d"), divisional_coupler_count);
			cfg.Write(m_group, buffer, value, true);
			break;
		}
	}

	cfg.Write(m_group, wxT("NumberOfStops"), (int)stop_count);
	cfg.Write(m_group, wxT("NumberOfCouplers"), (int)coupler_count);
	cfg.Write(m_group, wxT("NumberOfTremulants"), (int)tremulant_count);
	cfg.Write(m_group, wxT("NumberOfDivisionalCouplers"), (int)divisional_coupler_count);
}
