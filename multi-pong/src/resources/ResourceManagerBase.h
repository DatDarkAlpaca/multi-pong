#pragma once
#include "pch.h"

namespace pong
{
	template<typename ResourceType>
	class ResourceManagerBase
	{
	public:
		virtual void Add(const std::string& resourceName, const std::string& filepath)
		{
			std::unique_ptr<ResourceType> resource = std::make_unique<ResourceType>();

			if (!resource->loadFromFile(filepath))
				return;

			m_Resources[resourceName] = std::move(resource);
		}

		virtual void Remove(const std::string& resourceName)
		{
			auto it = m_Resources.find(resourceName);
			if (it != m_Resources.end())
				it = m_Resources.erase(it);
		}

		virtual ResourceType* Get(const std::string& name)
		{
			auto it = m_Resources.find(name);
			if (it != m_Resources.end())
				return it->second.get();

			return nullptr;
		}

	protected:
		std::unordered_map<std::string, std::unique_ptr<ResourceType>> m_Resources;
	};
}