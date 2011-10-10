/*
Copyright (c) 2006-2009, Fred Emmott <mail@fredemmott.co.uk>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#include "sdljoystick.h"

#include <QBasicTimer>
#include <QDebug>

#define POLL_INTERVAL 20

SdlJoystick::SdlJoystick(int js)
{
    // Sure, we're only using the Joystick, but SDL doesn't work if video isn't initialised
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    m_joystick = 0;
    if(js >=0) {
        Q_ASSERT(js < attachedJoysticks());
        Q_ASSERT(js >= 0);

        m_joystick = SDL_JoystickOpen(js);
    }
    m_timer = new QBasicTimer();
    m_timer->start(POLL_INTERVAL, this);
}

int SdlJoystick::currentJoystick()
{
    return SDL_JoystickIndex(m_joystick);
}

QString SdlJoystick::joystickName(int js)
{
    Q_ASSERT(js < attachedJoysticks());
    Q_ASSERT(js >= 0);
    return QString(SDL_JoystickName(js));
}

void SdlJoystick::setJoystick(int js)
{
    Q_ASSERT(js < attachedJoysticks());
    Q_ASSERT(js >= 0);

    if(m_joystick)
        SDL_JoystickClose(m_joystick);
    m_joystick = SDL_JoystickOpen(js);
}

SdlJoystick::~SdlJoystick()
{
    delete m_timer;
    if(m_joystick)
        SDL_JoystickClose(m_joystick);
    SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
}

void SdlJoystick::timerEvent(QTimerEvent* ignored)
{
    if(!m_joystick) return;
    SDL_Event event;

    m_timer->stop();
    while ( SDL_PollEvent(&event) )
    {
        switch(event.type)
        {
        case SDL_JOYAXISMOTION:
            emit axisEvent(event.jaxis.axis, event.jaxis.value);
            break;
        case SDL_JOYBUTTONDOWN:
            emit buttonPress(event.jbutton.button);
            break;
        case SDL_JOYBUTTONUP:
            emit buttonRelease(event.jbutton.button);
            break;
        }
    }
    m_timer->start(POLL_INTERVAL, this);
}

int SdlJoystick::attachedJoysticks()
{
    return SDL_NumJoysticks();
}
