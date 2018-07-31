/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_ELEMENTNAME_H
#define LIBREPCB_ELEMENTNAME_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <type_safe/constrained_type.hpp>
#include "fileio/sexpression.h"

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {

/*****************************************************************************************
 *  Class ElementName
 ****************************************************************************************/

struct ElementNameVerifier {
    template <typename Value, typename Predicate>
    static constexpr auto verify(Value&& val, const Predicate& p) -> typename std::decay<Value>::type {
        return p(val) ? std::forward<Value>(val) : (
            throw RuntimeError(__FILE__, __LINE__, QString(
                QApplication::translate("ElementName", "Invalid name: '%1'")).arg(val)),
            std::forward<Value>(val));
    }
};

struct ElementNameConstraint {
    bool operator()(const QString& value) const noexcept {
        if (value.isEmpty()) return false;
        if (value.length() > 70) return false;
        if (value != value.trimmed()) return false;
        foreach (const QChar& c, value) {if (!c.isPrint()) return false;}
        return true;
    }
};

/**
 * ElementName is a wrapper around QString which guarantees to contain a valid element
 * name (used as name for several objects)
 *
 * An element name is considered as valid if it:
 *   - contains minimum 1 and maximum 70 characters
 *   - contains only printable characters
 *   - does not start or end with whitespaces
 *
 * The constructor throws an exception if constructed from a QString which is not a valid
 * element name according these rules.
 */
using ElementName = type_safe::constrained_type<QString, ElementNameConstraint,
                                                 ElementNameVerifier>;

inline bool operator==(const ElementName& lhs, const QString& rhs) noexcept {return (*lhs) == rhs;}
inline bool operator==(const QString& lhs, const ElementName& rhs) noexcept {return lhs == (*rhs);}
inline bool operator!=(const ElementName& lhs, const QString& rhs) noexcept {return (*lhs) != rhs;}
inline bool operator!=(const QString& lhs, const ElementName& rhs) noexcept {return lhs != (*rhs);}
inline QString operator%(const ElementName& lhs, const QString& rhs) noexcept {return (*lhs) % rhs;}
inline QString operator%(const QString& lhs, const ElementName& rhs) noexcept {return lhs % (*rhs);}
inline ElementName operator%(const ElementName& lhs, const ElementName& rhs) noexcept {
    return ElementName((*lhs) % (*rhs)); // always safe, will not throw
}

template <>
inline SExpression serializeToSExpression(const ElementName& obj) {
    return SExpression::createString(*obj);
}

template <>
inline ElementName deserializeFromSExpression(const SExpression& sexpr, bool throwIfEmpty) {
    QString str = sexpr.getStringOrToken(throwIfEmpty);
    // backward compatibility - remove this some time!
    str = str.trimmed();
    str.truncate(70);
    return ElementName(str); // can throw
}

inline QDataStream& operator<<(QDataStream& stream, const ElementName& obj) {
    stream << *obj;
    return stream;
}

inline QDebug operator<<(QDebug stream, const ElementName& obj) {
    stream << QString("ElementName('%1'')").arg(*obj);
    return stream;
}

inline uint qHash(const ElementName& key, uint seed = 0) noexcept {
    return ::qHash(*key, seed);
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace librepcb

#endif // LIBREPCB_ELEMENTNAME_H
