#ifndef IALGORITHM_HH
#define IALGORITHM_HH

#include "SequenceElement.hh"
#include <Acts/Utilities/Logger.hpp>
#include "ProcessCode.hh"

class IAlgorithm : public SequenceElement
{
public:
    IAlgorithm(std::string name,
               Acts::Logging::Level level = Acts::Logging::INFO);
    /// The algorithm name.
    std::string name() const override;

    /// Execute the algorithm for one event.
    ///
    /// This function must be implemented by subclasses.
    virtual ProcessCode execute(const AlgorithmContext &context) const = 0;

    /// Internal execute method forwards to the algorithm execute method as const
    /// @param context The algorithm context
    ProcessCode internalExecute(const AlgorithmContext &context) final
    {
        return execute(context);
    };

    /// Initialize the algorithm
    ProcessCode initialize() override { return ProcessCode::SUCCESS; }
    /// Finalize the algorithm
    ProcessCode finalize() override { return ProcessCode::SUCCESS; }

protected:
    const Acts::Logger &logger() const { return *m_logger; }

private:
    std::string m_name;
    std::unique_ptr<const Acts::Logger> m_logger;
};

#endif