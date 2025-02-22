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

#ifndef MU_VST_VSTPLUGINSREGISTER_H
#define MU_VST_VSTPLUGINSREGISTER_H

#include <map>
#include <unordered_map>
#include <mutex>

#include "modularity/ioc.h"
#include "audio/iaudiothreadsecurer.h"

#include "ivstpluginsregister.h"

namespace mu::vst {
class VstPluginsRegister : public IVstPluginsRegister
{
    INJECT_STATIC(vst, audio::IAudioThreadSecurer, threadSecurer)
public:
    void registerInstrPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId, VstPluginPtr pluginPtr) override;
    void registerFxPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId, VstPluginPtr pluginPtr) override;
    void registerMasterFxPlugin(const audio::AudioResourceId& resourceId, VstPluginPtr pluginPtr) override;

    VstPluginPtr instrumentPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId) const override;
    VstPluginPtr fxPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId) const override;
    VstPluginPtr masterFxPlugin(const audio::AudioResourceId& resourceId) const override;

    void unregisterInstrPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId) override;
    void unregisterFxPlugin(const audio::TrackId trackId, const audio::AudioResourceId& resourceId) override;
    void unregisterMasterFxPlugin(const audio::AudioResourceId& resourceId) override;

private:
    mutable std::mutex m_mutex;

    using PluginInstancesMap = std::unordered_map<audio::AudioResourceId, VstPluginPtr>;

    std::map<audio::TrackId, PluginInstancesMap> m_vstiPluginsMap;
    std::map<audio::TrackId, PluginInstancesMap> m_vstFxPluginsMap;
    PluginInstancesMap m_masterPluginsMap;
};
}

#endif // MU_VST_VSTPLUGINSREGISTER_H
