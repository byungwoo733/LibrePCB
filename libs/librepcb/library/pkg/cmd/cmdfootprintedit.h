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

#ifndef LIBREPCB_LIBRARY_CMDFOOTPRINTEDIT_H
#define LIBREPCB_LIBRARY_CMDFOOTPRINTEDIT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <librepcb/common/undocommand.h>
#include <librepcb/common/elementname.h>

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {
namespace library {

class Footprint;

/*****************************************************************************************
 *  Class CmdFootprintEdit
 ****************************************************************************************/

/**
 * @brief The CmdFootprintEdit class
 */
class CmdFootprintEdit final : public UndoCommand
{
    public:

        // Constructors / Destructor
        CmdFootprintEdit() = delete;
        CmdFootprintEdit(const CmdFootprintEdit& other) = delete;
        explicit CmdFootprintEdit(Footprint& fpt) noexcept;
        ~CmdFootprintEdit() noexcept;

        // Setters
        void setName(const ElementName& name) noexcept;

        // Operator Overloadings
        CmdFootprintEdit& operator=(const CmdFootprintEdit& rhs) = delete;


    private:

        // Private Methods

        /// @copydoc UndoCommand::performExecute()
        bool performExecute() override;

        /// @copydoc UndoCommand::performUndo()
        void performUndo() override;

        /// @copydoc UndoCommand::performRedo()
        void performRedo() override;


        // Private Member Variables

        // Attributes from the constructor
        Footprint& mFootprint;

        // General Attributes
        ElementName mOldName;
        ElementName mNewName;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace library
} // namespace librepcb

#endif // LIBREPCB_LIBRARY_CMDFOOTPRINTEDIT_H
