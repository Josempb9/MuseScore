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

#include "synthresolver.h"

#include "log.h"
#include "async/async.h"

#include "audioerrors.h"
#include "internal/audiothread.h"
#include "internal/audiosanitizer.h"

using namespace mu::async;
using namespace mu::audio;
using namespace mu::audio::synth;

void SynthResolver::init(const AudioInputParams& defaultInputParams)
{
    ONLY_AUDIO_WORKER_THREAD;

    IF_ASSERT_FAILED(defaultInputParams.isValid()) {
        return;
    }

    m_defaultInputParams = defaultInputParams;
}

ISynthesizerPtr SynthResolver::resolveSynth(const TrackId trackId, const AudioInputParams& params) const
{
    ONLY_AUDIO_WORKER_THREAD;

    TRACEFUNC;

    IF_ASSERT_FAILED(params.isValid()) {
        LOGE() << "invalid audio source params for trackId: " << trackId;
        return nullptr;
    }

    std::lock_guard lock(m_mutex);

    auto search = m_resolvers.find(params.type);

    if (search == m_resolvers.end()) {
        return nullptr;
    }

    const IResolverPtr& resolver = search->second;
    return resolver->resolveSynth(trackId, params.resourceId);
}

ISynthesizerPtr SynthResolver::resolveDefaultSynth(const TrackId trackId) const
{
    ONLY_AUDIO_WORKER_THREAD;

    return resolveSynth(trackId, m_defaultInputParams);
}

AudioResourceIdList SynthResolver::resolveAvailableResources(const AudioSourceType type) const
{
    ONLY_AUDIO_WORKER_THREAD;

    TRACEFUNC;

    std::lock_guard lock(m_mutex);

    auto search = m_resolvers.find(type);

    if (search != m_resolvers.end()) {
        return search->second->resolveResources();
    }

    LOGE() << "Unable to find a resolver for type: " << static_cast<int>(type);
    return {};
}

void SynthResolver::registerResolver(const AudioSourceType type, IResolverPtr resolver)
{
    ONLY_AUDIO_MAIN_OR_WORKER_THREAD;

    std::lock_guard lock(m_mutex);

    m_resolvers.insert_or_assign(type, std::move(resolver));
}
