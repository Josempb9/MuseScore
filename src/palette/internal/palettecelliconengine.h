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
#ifndef MU_PALETTE_PALETTECELLICONENGINE_H
#define MU_PALETTE_PALETTECELLICONENGINE_H

#include <QIconEngine>

#include "palettecell.h"

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"
#include "ui/iuiconfiguration.h"

namespace mu::palette {
class PaletteCellIconEngine : public QIconEngine
{
    INJECT_STATIC(palette, IPaletteConfiguration, configuration)
    INJECT_STATIC(palette, ui::IUiConfiguration, uiConfiguration)

public:
    explicit PaletteCellIconEngine(PaletteCellConstPtr cell, qreal extraMag = 1.0);

    QIconEngine* clone() const override;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;

    static void paintPaletteElement(void* data, Ms::Element* element);

private:
    void paintCell(draw::Painter& painter, const RectF& rect, bool selected, bool current) const;
    void paintBackground(draw::Painter& painter, const RectF& rect, bool selected, bool current) const;
    void paintActionIcon(draw::Painter& painter, const RectF& rect, Ms::Element* element) const;
    qreal paintStaff(draw::Painter& painter, const RectF& rect, qreal spatium) const;
    void paintScoreElement(draw::Painter& painter, Ms::Element* element, qreal spatium, bool alignToStaff) const;

    PaletteCellConstPtr m_cell;
    qreal m_extraMag = 1.0;
};
}

#endif // MU_PALETTE_PALETTECELLICONENGINE_H
