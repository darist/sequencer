/**
   Tests for the sequencer lib.
   TODO: Use something like ArduinoUnit instead of rolling our own.
*/
#include <sequencer.h>
#include <limits.h>


#define LED 13

int x = 0;
int y = 0;
int failedTests = 0; // pass

void hello() {
  x++;
}

void hello2() {
  y++;
}

// Some simple test sequences
SequenceEvent oneEvent[] = {
  {0, &hello}
};

SequenceEvent twoEvents[] = {
  {100000, &hello},
  {300000, &hello}
};

// Pointer to a looping sequence.
Sequence* loopingSequence;

void fail(String msg) {           // FA: fail function without line number
    fail(msg, -1);
}

void fail(String msg, int line) {  // FA: include line number in fail fun.
  Serial.print(" FAILED(");
  Serial.print(line);
  Serial.print("): ");
  Serial.println(msg);
  failedTests++;
  // TODO: find a way to throw or print a stack trace.
}


void testSequence() {
  Serial.println(F("Starting testSequence..."));
  x = 0;
  Sequence s(oneEvent, 1);
  s.start();
  // Nothing should happen before tick
  // TODO: or should the event fire if it was set for 0 time?
  if (x != 0) {
    fail(F("hello() was called too early"), __LINE__);
  }
  // Call tick() -- it should trigger a call to hello().
  s.tick();
  // check that hello() was called
  if (x != 1) {
    fail(F("Expected 1. Got something else."), __LINE__);
  }
}

void testSequence_tickBeforeStart() {
  Serial.println(F("Starting testSequence_tickBeforeStart..."));
  x = 0;
  Sequence s(oneEvent, 1);
  // Don't call start, just tick a few times.
  s.tick();
  s.tick();
  s.tick();
  // check that hello() was never called.
  if (x != 0) {
    fail(F("Expected hello() not to be called, but it was called."), __LINE__);
  }
}

void testSequence_fakeTime() {
  Serial.println(F("Starting testSequence_fakeTime..."));
  x = 0;
  Sequence s(twoEvents, 2);
  s.start(1000000);
  if (x != 0) {
    fail(F("hello() should not be called yet."), __LINE__);
  }
  s.tick(1099999);
  if (x != 0) {
    fail(F("hello() should not be called yet."), __LINE__);
  }
  s.tick(1100000);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  s.tick(1100001);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  s.tick(1399999);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  if (!s.running()) {
    fail(F("Sequence should still be running."), __LINE__);
  }
  s.tick(1400000);
  if (x != 2) {
    fail(F("hello() should be called twice."), __LINE__);
  }
  if (s.running()) {
    fail(F("Sequence should have stopped running."), __LINE__);
  }
}

void testSequence_restart() {
  Serial.println(F("Starting testSequence_restart..."));
  x = 0;
  Sequence s(twoEvents, 2);
  s.start(0);
  s.tick(100000);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  if (!s.running()) {
    fail(F("Sequence should still be running."), __LINE__);
  }

  // Restart the sequence.
  s.start(200000);
  if (!s.running()) {   // FA: negation added: (!)
    fail(F("Sequence should continue running."), __LINE__);
  }
  // We should see another event in 100ms.
  s.tick(299999);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  s.tick(300000);
  if (x != 2) {
    fail(F("hello() should be called twice."), __LINE__);
  }
  // And the next event should happen 300ms later.
  s.tick(599999);
  if (x != 2) {
    fail(F("hello() should be called twice."), __LINE__);
  }
  s.tick(600000);
  if (x != 3) {
    fail(F("hello() should be called three times."), __LINE__);
  }
}

void testSequence_differentCallbacks() {
  Serial.println(F("Starting testSequence_differentCallbacks..."));
  // The point here is to make sure the right method is called.
  // eg: no off-by-one errors on indexing the sequence.
  x = 0;
  y = 0;
  SequenceEvent hellos[] = {
    {1000, &hello},
    {2000, &hello2},
    {3000, &hello},
    {4000, &hello},
    {5000, &hello2},
    {6000, &hello2}
  };
  Sequence s(hellos, 6);
  s.start(0);
  
  // 1000us later.
  s.tick(1000);
  if (!(x == 1 && y == 0)) { fail(F("Wrong function was called."), __LINE__); }
  
  // 2000us later:
  s.tick(2000);
  if (!(x == 1 && y == 0)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(3000);
  if (!(x == 1 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }

  // 3000us later:
  s.tick(4000);
  if (!(x == 1 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(5000);
  if (!(x == 1 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(6000);
  if (!(x == 2 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }

  // 4000us later:
  s.tick(7000);
  if (!(x == 2 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(8000);
  if (!(x == 2 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(9000);
  if (!(x == 2 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
  s.tick(10000);
  if (!(x == 3 && y == 1)) { fail(F("Wrong function was called."), __LINE__); }
}

void restart() {
  loopingSequence->start();
};

void testSequence_selfRestart() {
  Serial.println(F("Starting testSequence_selfRestart..."));
  // This test is intended as an example for how to make sequences
  // that repeat themselves.
  // TODO: This is looking very combersome: we should find a better
  // way to make sequences that repeat indefinetly, or for a predefined
  // number of times.
  x = 0;
  SequenceEvent tailEvents[] = {
    {1000, &hello},
    {2000, &restart}
  };
  Sequence s(tailEvents, 2);
  loopingSequence = &s;
  s.start();
  // Using real time, instead of fake because it's hard to pass
  // a fake time through the restart() function.

  // 1000us later, the first event should fire:
  delay(1);
  s.tick();
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  // Next event (restart) should happen 2000us later.
  delay(2);
  s.tick();
  // Sequence should restart.
  if (!s.running()) {
    fail(F("Sequence should still be running."), __LINE__);
  }
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  // 1000us later, the first event should fire again.
  delay(1);
  s.tick();
  if (x != 2) {
    fail(F("hello() should be called twice."), __LINE__);
  }
}

void testSequence_overflow() {
  Serial.println(F("Starting testSequence_overflow..."));
  x = 0;
  SequenceEvent events[] = {
    {10, &hello},
    {20, &hello}
  };
  Sequence s(events, 2);
  s.start(ULONG_MAX - 1);
  // First event should happen 10us later (after overflow, that is at 8us).
  s.tick(7);
  if (x != 0) {
    fail(F("hello() should not be called yet."), __LINE__);
  }
  s.tick(8);
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
  // Next event should happen at 28us.
  s.tick(27);
  if (x != 1) {
    fail(F("hello() should called once."), __LINE__);
  }
  s.tick(28);
  if (x != 2) {
    fail(F("hello() should be called twice."), __LINE__);
  }
}

void testSequence_largeDelay() {
  Serial.println(F("Starting testSequence_largeDelay..."));
  x = 0;
  Sequence s(twoEvents, 2);
  s.start(1000);
  s.tick(3000000000ul); // The delta overflows a signed long.
  if (x != 1) {
    fail(F("hello() should be called once."), __LINE__);
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(9600);

  // Run tests methods:
  testSequence();
  testSequence_tickBeforeStart();
  testSequence_fakeTime();
  testSequence_restart();
  testSequence_selfRestart();
  testSequence_overflow();
  testSequence_largeDelay();
  testSequence_differentCallbacks();

  // Summary:
  Serial.println(F("Done."));
  Serial.print(failedTests);
  Serial.println(F(" tests failed."));
}

void loop() {
  // Blink LED if tests failed.
  if (failedTests > 0) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  } else {
    digitalWrite(LED, HIGH);
  }
}

