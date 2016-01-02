#ifndef SEQUENCER_H
#define SEQUENCER_H

struct SequenceEvent {
  unsigned long timeout;
  void (*callback)(void);  // Function pointer for functions of the type void foo(void)
};

class Sequence {
  public:
    Sequence(SequenceEvent* eventsList, int eventsLength);

    /** Kicks off the sequence. */
    void start();
    void start(unsigned long time);

    /** Returns true if the sequence is running. */
    bool running();

    /**
     * Called on every iteration of the loop(). Use Sequence helper instead
     * if several sequences are active at the same time. Uses micros() to determine
     * current time.
     */
    void tick();

    /**
     * Called on every iteration of the loop(). Accepts a timestamp in 
     * microseconds.
     * Useful for testing, or when you want to pass the same timestamp to all
     * sequences for each iteration of the loop().
     */
    void tick(unsigned long time);

  private:
    SequenceEvent *events;
    int eventCount;
    int nextEvent;
    bool started;
    unsigned long lastEventTime; /* us */
};

class SequenceHelper {
  public:
    /** 
     * Initializes a SequenceHelper for a max of the given number of Sequences.
     * Params:
     *   s: Array of pointers to Sequence.
     *   c: Number of squences in the array.
     */
    SequenceHelper(Sequence **s, int c);

    /** Returns true if all sequences are done. */
    bool done();

    /** This function should be called on every iteration of the loop(). */
    void tick();
  
  private:
    Sequence **sequences; // Array of pointers to Sequence.
    int count;
};

#endif

