#pragma once

namespace sh
{
    /** \ingroup group_framework
     * The entrypoint of the engine.
     *
     * This class is the framework that the engine runs on. It is responsible
     * for starting and shutting down subsystems.
     */
    class Engine
    {
    public:

        /**
         * Start the loop of the engine.
         *
         * This function has to be called to start up the engine
         */
        void Run();
    };
}