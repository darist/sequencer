#include "arduino.h"
#include "sequencer.h"

/*
 * Sequence implementation.
 */
Sequence::Sequence(SequenceEvent *eventsList, int eventsLength) {
    events = eventsList;
    eventCount = eventsLength;
    nextEvent = 0;
    started = false;
    lastEventTime = 0;
}

void Sequence::start() {
    start(micros());
}

void Sequence::start(unsigned long time) {
    nextEvent = 0;
    started = true;
    lastEventTime = time;
}

bool Sequence::running() {
    return started;
}

void Sequence::tick() {
    tick(micros());
}

void Sequence::tick(unsigned long time) {
    // Don't do anything if sequence has not been started.
    if (!started) {
        return;
    }

    // Compute elapsed time since the last event (or start).
    // Note: Using unsigned longs works even in the event of overflow.
    // eg: if the last event was at ULONG_MAX, and it's now 100, the result of
    // 100 - ULONG_MAX = 101
    unsigned long elapsed = time - lastEventTime;
    if (elapsed >= events[nextEvent].timeout) {  // FA: changed to >=
        // Time to fire event - update our state first, then fire the event
        // in case the event affects the state (e.g.: by calling start()).
        lastEventTime = time;
        int event = nextEvent;
        nextEvent++;
        if (nextEvent >= eventCount) {
            // We are done with all events.
            started = false;
            nextEvent = 0;
        }
        
        // Now actually call the event callback.
        events[event].callback();
    }
}

/*
 * SequenceHelper implementation.
 */

SequenceHelper::SequenceHelper(Sequence **s, int len) {
    sequences = s;
    count = len;
}

bool SequenceHelper::done() {
    // If any one sequence is still running, return false.
    for (int i = 0; i < count; i++) {
        if (sequences[i]->running()) {
            return false;
        }
    }
    return true;
}

void SequenceHelper::tick() {
    // Tick all the sequences.
    for (int i = 0; i < count; i++) {
        sequences[i]->tick();
    }
}

