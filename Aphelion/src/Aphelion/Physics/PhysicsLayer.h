#pragma once
#include "Aphelion/Core/Core.h"
#include "Aphelion/Core/Log.h"
#include "Aphelion/Core/Layer.h"

#include "Aphelion/Physics/PhysicsFoundation.h"

#include <thread>
#include <string>

namespace ap
{
    /**
     * This layer initializes the Physics subsystem
     * Meant to be used by the Aphelion Engine. 
     */
    class APHELION_API PhysicsLayer : public Layer
    {
    public:
        PhysicsLayer()
            : Layer("Physics") {}

        virtual void OnAttach() override final 
        {
            PhysicsFoundationDesc desc;
            desc.logCb = [](ap::PhysicsErrorCode code, const char* message, const char* file, int line)
            {
                std::string type = "n/a";

                switch (code)
                {
                case ap::PhysicsErrorCode::NoError:
                    type = "NoError";
                    AP_CORE_TRACE("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;

                case ap::PhysicsErrorCode::DebugInfo:
                    type = "DebugInfo";
                    AP_CORE_INFO("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;

                case ap::PhysicsErrorCode::DebugWarning:
                    type = "DebugWarning";
                case ap::PhysicsErrorCode::PerfWarning:
                    type = "PerfWarning";
                case ap::PhysicsErrorCode::InvalidParameter:
                    type = "InvalidParameter";
                case ap::PhysicsErrorCode::InvalidOperation:
                    type = "InvalidOperation";
                    AP_CORE_WARN("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;

                case ap::PhysicsErrorCode::OutOfMemory:
                    type = "OutOfMemory";
                case ap::PhysicsErrorCode::InternalError:
                    type = "InternalError";
                    AP_CORE_ERROR("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;
                case ap::PhysicsErrorCode::Abort:
                    type = "Abort";
                    AP_CORE_CRITICAL("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;

                case ap::PhysicsErrorCode::MaskAll:
                    type = "MaskAll";
                    AP_CORE_TRACE("Physics {} File: {} Line: {}] {}", type, file, line, message);
                    break;
                }
            };
            desc.cores = std::thread::hardware_concurrency();

            PhysicsFoundation::Init(desc);
        }

        virtual void OnDetach() override final 
        {
            PhysicsFoundation::Deinit();
        }
        
        virtual void OnEvent(Event& event) override final 
        {
        }
        
        virtual void OnUpdate(Timestep ts) override final 
        {
        }
        
        virtual void OnGuiRender() override final 
        {
        }
    };
}