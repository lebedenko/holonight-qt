// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 Andrii L <lebeden@gmail.com>

#include "holoniightpalette.h"

#include "appearancecontext.h"
#include "holonight/appearance_reader.h"
#include "themeresolver.h"

HoloniightPalette::HoloniightPalette(Holonight::AppearanceReader* reader, QObject* parent)
    : QObject{parent}, reader_{reader}, tok_{Holonight::ThemeResolver::resolve(reader->appearance())} {
  connect(reader_, &Holonight::AppearanceReader::paletteChanged, this, [this] {
    tok_ = Holonight::ThemeResolver::resolve(reader_->appearance());
    ++revision_;
    Q_EMIT paletteChanged();
  });
}

HoloniightPalette* HoloniightPalette::create(QQmlEngine* engine, QJSEngine* script_engine) {
  Q_UNUSED(script_engine)
  return new HoloniightPalette{Holonight::appearanceReaderForEngine(engine), engine};
}

void HoloniightPalette::reload() { reader_->reload(); }
