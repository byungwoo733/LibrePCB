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

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "application.h"
#include "exceptions.h"
#include "dialogs/aboutdialog.h"
#include "font/strokefontpool.h"
#include "units/all_length_units.h"

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

Application::Application(int& argc, char** argv) noexcept :
    QApplication(argc, argv),
    mAppVersion(Version::fromString(APP_VERSION)),
    mGitVersion(GIT_VERSION),
    mFileFormatVersion(Version::fromString(FILE_FORMAT_VERSION))
{
    // register meta types
    qRegisterMetaType<FilePath>();
    qRegisterMetaType<Point>();

    // set application version
    QApplication::setApplicationVersion(mAppVersion.toPrettyStr(2));

    // set build timestamp
    QDate buildDate = QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy"));
    QTime buildTime = QTime::fromString(__TIME__, Qt::TextDate);
    mBuildDate = QDateTime(buildDate, buildTime);

    // check "git describe" version
    if (mGitVersion.isEmpty()) {
        qWarning() << "Git revision not compiled into the executable!";
    }

    // check file format version
    if (!mFileFormatVersion.isPrefixOf(mAppVersion)) {
        qFatal("The file format version is not a prefix of the application version!");
    }

    // get the directory of the currently running executable
    FilePath executableFilePath(QApplication::applicationFilePath());
    Q_ASSERT(executableFilePath.isValid());

    // determine the path to the resources directory (e.g. /usr/share/librepcb)
    FilePath buildOutputDirPath(BUILD_OUTPUT_DIRECTORY);
    bool runningFromBuildOutput = executableFilePath.isLocatedInDir(buildOutputDirPath);
    if (runningFromBuildOutput) {
        // The executable is located inside the build output directory, so we assume this
        // is a developer build and thus we use the "share" directory from the repository
        // root.
        mResourcesDir = FilePath(SHARE_DIRECTORY_SOURCE).getPathTo("librepcb");
    } else {
        // The executable is located outside the build output directory, so we assume this
        // is a packaged build and thus we use the "share" directory which is bundled with
        // the application (must be located at "../share" relative to the executable).
        mResourcesDir = executableFilePath.getParentDir().getPathTo("../share/librepcb");
    }

    // warn if runtime resource files are not found
    if (!getResourcesFilePath("README.md").isExistingFile()) {
        qCritical() << "Could not find resource files! Probably packaging went wrong?!";
        qCritical() << "Expected resources location:" << mResourcesDir.toNative();
        qCritical() << "Executable location:        " << executableFilePath.toNative();
        qCritical() << "Build output directory:     " << buildOutputDirPath.toNative();
        qCritical() << "Is developer build:         " << runningFromBuildOutput;
    }

    // load all bundled TrueType/OpenType fonts
    QDir fontsDir(getResourcesFilePath("fonts").toStr());
    fontsDir.setFilter(QDir::Files);
    fontsDir.setNameFilters({"*.ttf", "*.otf"});
    foreach (const QFileInfo& info, fontsDir.entryInfoList()) {
        QString fp = info.absoluteFilePath();
        int id = QFontDatabase::addApplicationFont(fp);
        if (id < 0) {
            qCritical() << "Failed to load font" << fp;
        }
    }

    // set default sans serif font
    mSansSerifFont.setStyleStrategy(QFont::StyleStrategy(QFont::OpenGLCompatible | QFont::PreferQuality));
    mSansSerifFont.setStyleHint(QFont::SansSerif);
    mSansSerifFont.setFamily("Noto Sans");

    // set default monospace font
    mMonospaceFont.setStyleStrategy(QFont::StyleStrategy(QFont::OpenGLCompatible | QFont::PreferQuality));
    mMonospaceFont.setStyleHint(QFont::TypeWriter);
    mMonospaceFont.setFamily("Noto Sans Mono");

    // load all stroke fonts
    mStrokeFontPool.reset(new StrokeFontPool(getResourcesFilePath("fontobene")));
    getDefaultStrokeFont(); // ensure that the default font is available (aborts if not)
}

Application::~Application() noexcept
{
}

/*****************************************************************************************
 *  Getters
 ****************************************************************************************/

FilePath Application::getResourcesFilePath(const QString& filepath) const noexcept
{
    return mResourcesDir.getPathTo(filepath);
}

const StrokeFont& Application::getDefaultStrokeFont() const noexcept
{
    try {
        return mStrokeFontPool->getFont(getDefaultStrokeFontName());
    } catch (const Exception& e) {
        qFatal("Default stroke font could not be loaded!"); // aborts the application!!!
    }
}

/*****************************************************************************************
 *  Reimplemented from QApplication
 ****************************************************************************************/

bool Application::notify(QObject* receiver, QEvent* e)
{
    try
    {
        return QApplication::notify(receiver, e);
    }
    catch (...)
    {
        qCritical() << "Exception caught in Application::notify()!";
    }
    return false;
}

/*****************************************************************************************
 *  Static Methods
 ****************************************************************************************/

Application* Application::instance() noexcept
{
    Application* app = dynamic_cast<Application*>(QCoreApplication::instance());
    Q_ASSERT(app);
    return app;
}

/*****************************************************************************************
 *  Slots
 ****************************************************************************************/

void Application::about() noexcept
{
    QWidget* parent = QApplication::activeWindow();
    AboutDialog aboutDialog(parent);
    aboutDialog.exec();
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace librepcb
