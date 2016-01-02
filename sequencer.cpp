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
    long elapsed = time - lastEventTime;                // FA: changed to "long", remove "unsigned"
    if (elapsed >= (long) events[nextEvent].timeout) {  // FA: changed to >=
        // Time to fire event:
        events[nextEvent].callback();
        
        // Adavance to next event.
        lastEventTime = time;
        nextEvent++;
        if (nextEvent >= eventCount) {
            // We are done with all events.
            started = false;
            nextEvent = 0;
        }
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

