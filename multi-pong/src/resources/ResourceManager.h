#pragma once
#include "pch.h"

namespace pong
{
	template<typename ResourceType>
	class ResourceManager
	{
	public:
		void Add(const std::string& resourceName, const std::string& filepath)
		{
			std::shared_ptr<ResourceType> resource = std::make_shared<ResourceType>();

			if (!resource->loadFromFile(filepath))
				return;

			m_Resources[resourceName] = std::move(resource);
		}

		void Remove(const std::string& resourceName)
		{
			auto it = m_Resources.find(resourceName);
			if (it != m_Resources.end())
				it = m_Resources.erase(it);
		}

		ResourceType* Get(const std::string& name)
		{
			auto it = m_Resources.find(name);
			if (it != m_Resources.end())
				return it->second.get();
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<ResourceType>> m_Resources;
	};
}