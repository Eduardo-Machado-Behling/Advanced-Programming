#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

struct Subscriber {
  virtual ~Subscriber() {}
  virtual void update() = 0;
};

#endif
