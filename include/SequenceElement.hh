#ifndef SEQUENCEELEMENT_HH
#define SEQUENCEELEMENT_HH
#include <string>
#include <vector>
#include "ProcessCode.hh"
#include "AlgorithmContext.hh"

class SequenceElement{
public:
  virtual ~SequenceElement() = default;

  /// The algorithm name.
  virtual std::string name() const = 0;

  /// Initialize the algorithm
  virtual ProcessCode initialize() = 0;

  /// Finalize the algorithm
  virtual ProcessCode finalize() = 0;

  /// Internal method to execute the algorithm for one event.
  /// @note Usually, you should not override this method
  virtual ProcessCode internalExecute(const AlgorithmContext& context) = 0;

//   const std::vector<const DataHandleBase*>& writeHandles() const;
//   const std::vector<const DataHandleBase*>& readHandles() const;

 private:
//   void registerWriteHandle(const DataHandleBase& handle);
//   void registerReadHandle(const DataHandleBase& handle);

//   template <typename T>
//   friend class WriteDataHandle;

//   template <typename T>
//   friend class ReadDataHandle;

//   std::vector<const DataHandleBase*> m_writeHandles;
//   std::vector<const DataHandleBase*> m_readHandles;
    
};


#endif