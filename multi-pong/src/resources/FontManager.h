#pragma once
#include "ResourceManagerBase.h"

namespace pong
{
	class FontManager : public ResourceManagerBase<sf::Font>
	{
	public:
		FontManager() = default;
		~FontManager() = default;

	public:
		static std::shared_ptr<FontManager>& GetInstance()
		{
			if (!s_Instance)
				s_Instance = std::shared_ptr<FontManager>(std::make_shared<FontManager>());

			return s_Instance;
		}

	private:
		static inline std::shared_ptr<FontManager> s_Instance = nullptr;
	};
}