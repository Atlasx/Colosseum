#pragma once

#include "Systems/EngineSystem.h"
#include "Systems/LogSystem.h"

#include "Input/Input.h"
#include "Input/InputAction.h"

#include "ObjectPool.h"

#include "stdlibincl.h"

class GLFWwindow;

namespace CE
{	
	/*
	* MISC NOTES
	* 
	* Keys, state changes between keys
	* multiple key bindings for the same action
	* accumulate held timers
	* 
	* thresholds for duration on held
	* continuous firing and discrete firing
	* 
	* Jump -> bound to space, instantaneous, on pressed
	* Interact -> bound to e, start timer on pressed, fire trigger on duration
	* WASD -> bound to WASD, continuously reporting on 2D axis, fires every frame
	* Mouse Delta -> bound to mouse move, accumulate and report every frame
	*	side note: consider mid-frame mouse input and partial rotation to input angle
	* 
	* Actions are composed of binding, trigger, callback
	* trigger is updated with key state deltas and per-frame updates
	* actions are checked every frame if trigger?
	* maybe actions call in to update their triggers and fire callback if trigger is met
	* actions can have callbacks with values, axis float values or others
	* 
	* input system manages list of actions, sends key state updates, frametime updates
	* should inputsystem delay callbacks until update tick?
	* 
	* input action maps key bindings to triggers, passing along update information
	*	carries and calls callback once trigger is met
	* 
	* input trigger checks if conditions to fire are met
	*	tracks duration
	*	tracks and matches state transitions (OnPressed, OnReleased)
	* 
	* pass a reference object or another way of getting input pre-post state
	* could pass a raw pointed back to the input system or a premade info struct
	* input system could keep previous frame data and only check inputs on update
	*	this would be simpler, but would miss out on any input between frames
	*	bad the more I think about it, taps between frames wouldn't be picked up at all
	* 
	* input system and triggers need to process every input, but maybe only send callbacks
	*	once per frame, at a specific time in the update cycle.
	* 
	*/

#ifdef CDEBUG
	class InputSystemDebug;
#endif

	class InputSystem;
	extern InputSystem* g_input;

	class InputSystem final : public EngineSystem
	{		
		using InputActionHandle = GenericHandle;

	public:
		InputSystem(Engine* engine) : EngineSystem(engine) {};
	
		/* CEngineSystem Interface */
	public:
		virtual std::string Name() const override { return "Input System"; }
	protected:
		virtual void Startup() override;
		virtual void Shutdown() override;
		friend class Engine;

#ifdef CDEBUG
		friend class InputSystemDebug;
#endif


		/* Input System API */
	public:
		void PollInput();
		void ProcessActions();
		void UpdateActions();
		
		template <typename T, typename... Args>
		InputActionHandle RegisterAction(std::string name, Args&&... args)
		{
			std::shared_ptr<InputAction> ptr = std::make_shared<T>(std::forward<Args>(args)...);

			InputActionHandle handle = m_actionPool.Create(ptr);
			if (handle == InputActionHandle::INVALID)
			{
				LOG_ERROR(INPUT, "Failed to create new InputActionHandle");
			}

			m_actionNames[handle] = name;

			return handle;
		}

		template <typename T>
		T* GetAction(const InputActionHandle& handle)
		{

		}

		bool RemoveAction(InputActionHandle actionHandle);

	private:
		GLFWwindow* m_window = nullptr;

		void OnCursorMoved(GLFWwindow* window, double xPos, double yPos);
		void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
		void OnScroll(GLFWwindow* window, double xOffset, double yOffset);
		void OnWindowClose(GLFWwindow* window);

		// Does this require thread mutex locks? input changes knowledge base while we're processing
		InputKnowledge m_inputKnowledge;

		const InputKnowledge& GetKnowledge()
		{
			return m_inputKnowledge;
		}

		// Manually updates our input knowledge state, used only on startup
		void UpdateInputKnowledge();

		// Update input knowledge for a KeyType key at a particular KeyState
		void UpdateKeyState(const KeyType key, const KeyState newState);
		
		// Update input knowledge for a mouse button with a keystate
		void UpdateMouseButtonState(const MouseButtonType button, const KeyState newState);

		ObjectPool<std::shared_ptr<InputAction>, 128, InputActionHandle> m_actionPool;
		std::queue<InputEvent> m_events;

		std::unordered_map<InputActionHandle, std::string> m_actionNames;

		/* Handle Global Input Processing */
	private:
		static void GOnCursorMoved(GLFWwindow* window, double xPos, double yPos)
		{
			g_input->OnCursorMoved(window, xPos, yPos);
		};

		static void GOnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			g_input->OnKey(window, key, scancode, action, mods);
		}

		static void GOnMouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			g_input->OnMouseButton(window, button, action, mods);
		}

		static void GOnScroll(GLFWwindow* window, double xOffset, double yOffset)
		{
			g_input->OnScroll(window, xOffset, yOffset);
		}

		static void GOnWindowClose(GLFWwindow* window)
		{
			g_input->OnWindowClose(window);
		}
	};
}