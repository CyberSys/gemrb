/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2023 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 */

#include "OverHeadText.h"

#include "Game.h"
#include "Interface.h"
#include "Scriptable.h"

#include "GUI/GameControl.h"

namespace GemRB {

const String& OverHeadText::GetText(size_t idx) const
{
	return messages[idx].text;
}

static constexpr int maxScrollOffset = 100;
void OverHeadText::SetText(String newText, bool display, bool append, const Color& newColor)
{
	size_t idx = 0;
	if (newText.empty()) {
		messages[idx].pos.Invalidate();
		Display(false, idx);
		return;
	}

	// always append for actors and areas ... unless it's the head hp ratio
	if (append && core->HasFeature(GF_ONSCREEN_TEXT) && (owner->Type == ST_ACTOR || owner->Type == ST_AREA)) {
		idx = messages.size();
		messages.emplace_back();
		if (owner->Type == ST_ACTOR) {
			messages[idx].scrollOffset = Point(0, maxScrollOffset);
		}
	} else {
		messages[idx].scrollOffset.Invalidate();
	}
	messages[idx].pos.Invalidate();
	messages[idx].text = std::move(newText);
	messages[idx].color = newColor;
	Display(display, idx);
}

bool OverHeadText::Display(bool show, size_t idx)
{
	if (show) {
		isDisplaying = true;
		messages[idx].timeStartDisplaying = core->Time.Ticks2Ms(core->GetGame()->GameTime);
		return true;
	} else if (isDisplaying) {
		// is this the last displaying message?
		if (messages.size() == 1) {
			isDisplaying = false;
			messages[idx].timeStartDisplaying = 0;
		} else {
			messages.erase(messages.begin() + idx);
			bool show = false;
			for (const auto& msg : messages) {
				show = show || msg.timeStartDisplaying != 0;
			}
			if (!show) isDisplaying = false;
		}
		return true;
	}
	return false;
}

// 'fix' the current overhead text - follow owner's position
void OverHeadText::FixPos(const Point& pos, size_t idx)
{
	messages[idx].pos = pos;
}

int OverHeadText::GetHeightOffset() const
{
	int offset = 100;
	if (owner->Type == ST_ACTOR) {
		offset = static_cast<const Selectable*>(owner)->circleSize * 50;
	}

	return offset;
}

void OverHeadText::Draw()
{
	if (!isDisplaying) {
		return;
	}

	int height = GetHeightOffset();
	bool show = false;
	for (auto msgIter = messages.begin(); msgIter != messages.end(); ++msgIter) {
		auto& msg = *msgIter;
		if (msg.timeStartDisplaying == 0) continue;
		if (msg.Draw(height, owner->Pos)) {
			show = true;
		} else if (msgIter != messages.begin()) { // always keep the one reserved slot
			msgIter = messages.erase(msgIter);
			--msgIter;
		}
	}

	// if all messages are done, mark as done
	if (!show) {
		isDisplaying = false;
	}
}

// *******************************************************************
// OverHeadMsg methods
bool OverHeadMsg::Draw(int heightOffset, const Point& fallbackPos)
{
	static constexpr tick_t maxDelay = 6000;
	tick_t delay = maxDelay;
	if (core->HasFeature(GF_ONSCREEN_TEXT) && !scrollOffset.IsInvalid()) {
		// empirically determined estimate to get the right speed and distance and 2px/tick
		delay = 1800;
	}

	tick_t time = core->Time.Ticks2Ms(core->GetGame()->GameTime);
	const Color& textColor = color == ColorBlack ? core->InfoTextColor : color;
	Font::PrintColors color = { textColor, ColorBlack };

	time -= timeStartDisplaying;
	if (time >= delay) {
		timeStartDisplaying = 0;
		return false;
	} else {
		time = (delay - time) / 10;
		// rapid fade-out
		if (time < 256) {
			color.fg.a = static_cast<unsigned char>(255 - time);
		}
	}

	Point p = pos.IsInvalid() ? fallbackPos : pos;
	Region vp = core->GetGameControl()->Viewport();
	// NOTE: in case we just printed a message, should we reduce the offset, so we can draw immediately without interference?
	Region rgn(p - Point(100, heightOffset) - vp.origin, Size(200, 400));
	if (delay != maxDelay) {
		rgn.y -= maxScrollOffset - scrollOffset.y;
		// rgn.h will be adjusted automatically, we don't need to worry about accidentally hiding other msgs
		scrollOffset.y -= 2;
	}
	core->GetTextFont()->Print(rgn, text, IE_FONT_ALIGN_CENTER | IE_FONT_ALIGN_TOP, color);

	return true;
}

}
