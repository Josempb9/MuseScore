/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "importmidi_tuplet_tonotes.h"

#include <QDebug>

#include "importmidi_tuplet.h"
#include "importmidi_fraction.h"
#include "importmidi_inner.h"
#include "libmscore/staff.h"
#include "libmscore/masterscore.h"
#include "libmscore/fraction.h"
#include "libmscore/duration.h"
#include "libmscore/measure.h"
#include "libmscore/tuplet.h"
#include "libmscore/mscore.h"
#include "libmscore/chordrest.h"

namespace Ms {
namespace MidiTuplet {
void addElementToTuplet(int voice,
                        const ReducedFraction& onTime,
                        const ReducedFraction& len,
                        DurationElement* el,
                        std::multimap<ReducedFraction, TupletData>& tuplets)
{
    const auto foundTuplets = findTupletsForTimeRange(voice, onTime, len, tuplets, true);

#ifdef QT_DEBUG
    if (foundTuplets.size() > 1) {
        qDebug() << "Measure number (from 1):" << el->measure()->no() + 1
                 << ", staff index (from 0):" << el->staff()->idx();

        Q_ASSERT_X(false, "MidiTuplet::addElementToTuplet",
                   "More than one tuplet contains specified duration");
    }
#endif

    if (!foundTuplets.empty()) {
        auto& tuplet = const_cast<TupletData&>(foundTuplets.front()->second);
        tuplet.elements.push_back(el);           // add chord/rest to the tuplet
    }
}

void createTupletNotes(
    Staff* staff,
    const std::multimap<ReducedFraction, TupletData>& tuplets)
{
    Score* score = staff->score();
    const int track = staff->idx() * VOICES;

    for (const auto& tupletEvent: tuplets) {
        const auto& tupletData = tupletEvent.second;
        if (tupletData.elements.empty()) {
            continue;
        }

        Tuplet* tuplet = new Tuplet(score);
        const auto& tupletRatio = tupletLimits(tupletData.tupletNumber).ratio;
        tuplet->setRatio(tupletRatio.fraction());

        tuplet->setTicks(tupletData.len.fraction());
        const TDuration baseLen = tupletData.len.fraction() / Fraction(tupletRatio.denominator(), 1);
        tuplet->setBaseLen(baseLen);

        tuplet->setTrack(track);
//            tuplet->setTick(tupletData.onTime.ticks());
        tuplet->setVoice(tupletData.voice);
        Measure* measure = score->tick2measure(tupletData.onTime.fraction());
        tuplet->setParent(measure);

        for (DurationElement* el: tupletData.elements) {
            tuplet->add(el);
            el->setTuplet(tuplet);
        }
    }
}

#ifdef QT_DEBUG

void printInvalidTupletLocation(int measureIndex, int staffIndex)
{
    qDebug() << "Tuplet is invalid; measure number (from 1):"
             << measureIndex + 1
             << ", staff index (from 0):" << staffIndex;
}

bool haveTupletsEnoughElements(const Staff* staff)
{
    const int strack = staff->idx() * VOICES;

    for (int voice = 0; voice < VOICES; ++voice) {
        for (Segment* seg = staff->score()->firstSegment(SegmentType::All); seg; seg = seg->next1()) {
            if (seg->segmentType() == SegmentType::ChordRest) {
                const ChordRest* cr = static_cast<ChordRest*>(seg->element(strack + voice));
                if (!cr) {
                    continue;
                }
                const Tuplet* tuplet = cr->tuplet();
                if (tuplet) {
                    if (tuplet->elements().size() <= 1) {
                        printInvalidTupletLocation(seg->measure()->no(), staff->idx());
                        return false;
                    }
                    int chordCount = 0;
                    for (const auto& e: tuplet->elements()) {
                        const ChordRest* cr1 = static_cast<ChordRest*>(e);
                        if (cr1 && cr1->isChord()) {
                            ++chordCount;
                        }
                    }
                    if (chordCount == 0) {
                        printInvalidTupletLocation(seg->measure()->no(), staff->idx());
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

#endif
} // namespace MidiTuplet
} // namespace Ms
