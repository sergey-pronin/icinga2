/******************************************************************************
 * Icinga 2                                                                   *
 * Copyright (C) 2012-2018 Icinga Development Team (https://www.icinga.com/)  *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License                *
 * as published by the Free Software Foundation; either version 2             *
 * of the License, or (at your option) any later version.                     *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program; if not, write to the Free Software Foundation     *
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.             *
 ******************************************************************************/

#ifndef CONFIGTYPE_H
#define CONFIGTYPE_H

#include "base/i2-base.hpp"
#include "base/object.hpp"
#include "base/type.hpp"
#include "base/dictionary.hpp"
#include "base/rw_spin_lock.hpp"

namespace icinga
{

class ConfigObject;

class ConfigType
{
public:
	virtual ~ConfigType(void);

	intrusive_ptr<ConfigObject> GetObject(const String& name) const;

	void RegisterObject(const intrusive_ptr<ConfigObject>& object);
	void UnregisterObject(const intrusive_ptr<ConfigObject>& object);

	const std::vector<intrusive_ptr<ConfigObject> >& GetObjectsUnlocked(void) const;
	rw_spin_lock& GetObjectsRWLock(void) const;

	template<typename T>
	static TypeImpl<T> *Get(void)
	{
		typedef TypeImpl<T> ObjType;
		return static_cast<ObjType *>(T::TypeInstance.get());
	}

	template<typename T>
	static std::vector<intrusive_ptr<T> > GetObjectsByType(void)
	{
		auto *ctype = CastTypeHelper(T::TypeInstance.get());
		RLock lock(ctype->GetObjectsRWLock());
		const std::vector<intrusive_ptr<ConfigObject> >& objects = ctype->GetObjectsUnlocked();

		std::vector<intrusive_ptr<T> > result;
		result.reserve(objects.size());
		for (const auto& object : objects) {
			result.push_back(static_pointer_cast<T>(object));
		}
		return result;
	}

	int GetObjectCount(void) const;

private:
	typedef std::map<String, intrusive_ptr<ConfigObject> > ObjectMap;
	typedef std::vector<intrusive_ptr<ConfigObject> > ObjectVector;

	mutable rw_spin_lock m_RWLock;
	ObjectMap m_ObjectMap;
	ObjectVector m_ObjectVector;

	static ConfigType *CastTypeHelper(Type *type);
};

}

#endif /* CONFIGTYPE_H */
