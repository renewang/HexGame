#ifndef MOCKLOCKABLEUTCPOLICY_H_
#define MOCKLOCKABLEUTCPOLICY_H_
#include "gmock/gmock.h"
class MockLockableUTCPolicy : public LockableUTCPolicy {
 public:
  MOCK_METHOD0(waitforupdate, bool());
  MOCK_METHOD0(notifyupdatedone, void());
  MOCK_METHOD0(OnDestroy, void());
  MOCK_METHOD0(OnConstruct, void());
  MockLockableUTCPolicy(): LockableUTCPolicy(){OnConstruct();}
  MockLockableUTCPolicy(const MockLockableUTCPolicy& other):LockableUTCPolicy(const_cast<MockLockableUTCPolicy&>(other)) {}
  MockLockableUTCPolicy& operator=(const MockLockableUTCPolicy& policy) {
    return *this;
  }
  virtual ~MockLockableUTCPolicy() { OnDestroy(); }
};
#endif /* MOCKLOCKABLEUTCPOLICY_H_ */
