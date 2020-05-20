#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;

public:

	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};

		Event()
			:
			type(Type::Invalid),
			code(0u) {}

		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code) {}

		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}

		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}

		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}

		unsigned char GetCode() const noexcept
		{
			return code;
		}
	private:
		Type type;
		unsigned char code;
	};

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// Key event handling
	bool IsKeyPressed(unsigned char keyCode) const noexcept;
	Event ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void FlushKey() noexcept;

	// Char event handling
	char ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;

	// Autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool IsAutorepeatEnabled() const noexcept;

private:
	void OnKeyPressed(unsigned char keyCode) noexcept;
	void OnKeyReleased(unsigned char keyCode) noexcept;
	void OnChar(char c) noexcept;
	void ClearState() noexcept;

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

	static constexpr unsigned int numberOfKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool isAutorepeatEnabled = false;
	std::bitset<numberOfKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};
