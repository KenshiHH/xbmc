/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://xbmc.org
 *
 *      Copyright (C) 2014-2015 Aracnoz
 *      http://github.com/aracnoz/xbmc
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "GUIDialogLAVVideo.h"
#include "Application.h"
#include "URL.h"
#include "dialogs/GUIDialogYesNo.h"
#include "filesystem/File.h"
#include "guilib/LocalizeStrings.h"
#include "profiles/ProfilesManager.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingsManager.h"
#include "utils/LangCodeExpander.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "utils/URIUtils.h"
#include "guilib/GUIWindowManager.h"
#include "dialogs/GUIDialogSelect.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "input/Key.h"
#include "utils/XMLUtils.h"
#include "Filters/RendererSettings.h"
#include "PixelShaderList.h"
#include "cores/playercorefactory/PlayerCoreFactory.h"
#include "Filters/LAVAudioSettings.h"
#include "Filters/LAVVideoSettings.h"
#include "Filters/LAVSplitterSettings.h"
#include "utils/CharsetConverter.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingsManager.h"
#include "addons/Skin.h"
#include "GraphFilters.h"

#define LAVVIDEO_PROPERTYPAGE      "lavvideo.propertypage"
#define LAVVIDEO_HWACCEL           "lavvideo.hwaccel"
#define LAVVIDEO_NUMTHREADS        "lavvideo.dwnumthreads"
#define LAVVIDEO_TRAYICON          "lavvideo.btrayicon"
#define LAVVIDEO_STREAMAR          "lavvideo.dwstreamar"
#define LAVVIDEO_DEINTFILEDORDER   "lavvideo.dwdeintfieldorder"
#define LAVVIDEO_DEINTMODE         "lavvideo.deintmode"
#define LAVVIDEO_RGBRANGE          "lavvideo.dwrgbrange"
#define LAVVIDEO_DITHERMODE        "lavvideo.dwdithermode"
#define LAVVIDEO_HWDEINTMODE       "lavvideo.dwhwdeintmode"
#define LAVVIDEO_HWDEINTOUT        "lavvideo.dwhwdeintoutput"
#define LAVVIDEO_HWDEINTHQ         "lavvideo.bhwdeinthq"
#define LAVVIDEO_SWDEINTMODE       "lavvideo.dwswdeintmode"
#define LAVVIDEO_SWDEINTOUT        "lavvideo.dwswdeintoutput"

using namespace std;

CGUIDialogLAVVideo::CGUIDialogLAVVideo()
  : CGUIDialogSettingsManualBase(WINDOW_DIALOG_LAVVIDEO, "VideoOSDSettings.xml")
{

}


CGUIDialogLAVVideo::~CGUIDialogLAVVideo()
{ }

void CGUIDialogLAVVideo::OnInitWindow()
{
  CGUIDialogSettingsManualBase::OnInitWindow();
}

void CGUIDialogLAVVideo::OnDeinitWindow(int nextWindowID)
{
  CGUIDialogSettingsManualBase::OnDeinitWindow(nextWindowID);
}

void CGUIDialogLAVVideo::Save()
{
}

void CGUIDialogLAVVideo::SetupView()
{
  CGUIDialogSettingsManualBase::SetupView();

  SetHeading(55077);
}

void CGUIDialogLAVVideo::InitializeSettings()
{
  CGUIDialogSettingsManualBase::InitializeSettings();

  bool usePopup = g_SkinInfo->HasSkinFile("DialogSlider.xml");

  CSettingCategory *category = AddCategory("dsplayerlavvideo", -1);
  if (category == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  // get all necessary setting groups
  CSettingGroup *groupProperty = AddGroup(category);
  if (groupProperty == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  CSettingGroup *group = AddGroup(category);
  if (group == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  // get all necessary setting groups
  CSettingGroup *groupSettings = AddGroup(category);
  if (groupSettings == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  // get all necessary setting groups
  CSettingGroup *groupOutput = AddGroup(category);
  if (groupOutput == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  // get all necessary setting groups
  CSettingGroup *groupDeintSW = AddGroup(category);
  if (groupDeintSW == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }
  // get all necessary setting groups
  CSettingGroup *groupDeintHW = AddGroup(category);
  if (groupDeintHW == NULL)
  {
    CLog::Log(LOGERROR, "CGUIDialogLAVVideo: unable to setup settings");
    return;
  }

  StaticIntegerSettingOptions entries;
  CLavSettings &LavSettings = CMediaSettings::Get().GetCurrentLavSettings();

  // BUTTON
  AddButton(groupProperty, LAVVIDEO_PROPERTYPAGE, 80013, 0);

  // MAIN
  entries.clear();
  for (unsigned int i = 0; i < 5; i++)
    entries.push_back(make_pair(80200 + i, i));
  AddList(group, LAVVIDEO_HWACCEL, 80005, 0, LavSettings.video_dwHWAccel, entries, 80005);

  entries.clear();
  for (unsigned int i = 0; i < 17; i++)
    entries.push_back(make_pair(80100 + i, i));
  AddList(group, LAVVIDEO_NUMTHREADS, 80003, 0, LavSettings.video_dwNumThreads, entries, 80003);

  AddToggle(group, LAVVIDEO_TRAYICON, 80001, 0, LavSettings.video_bTrayIcon);

  // SETTINGS
  AddToggle(groupSettings, LAVVIDEO_STREAMAR, 80002, 0, LavSettings.video_dwStreamAR);

  entries.clear();
  entries.push_back(make_pair(80100, 0));
  entries.push_back(make_pair(80205, 1));
  entries.push_back(make_pair(80206, 2));
  AddList(groupSettings, LAVVIDEO_DEINTFILEDORDER, 80009, 0, LavSettings.video_dwDeintFieldOrder, entries, 80009);

  entries.clear();
  entries.push_back(make_pair(80100, 0));
  entries.push_back(make_pair(80207, 1));
  entries.push_back(make_pair(80208, 2));
  entries.push_back(make_pair(80209, 3));
  AddList(groupSettings, LAVVIDEO_DEINTMODE, 80010, 0, (LAVDeintMode)LavSettings.video_deintMode, entries, 80010);

  // OUTPUT RANGE
  entries.clear();
  entries.push_back(make_pair(80214, 1));
  entries.push_back(make_pair(80215, 2));
  entries.push_back(make_pair(80216, 0));
  AddList(groupOutput, LAVVIDEO_RGBRANGE, 80004, 0, LavSettings.video_dwRGBRange, entries, 80004);

  entries.clear();
  entries.push_back(make_pair(80212, 0));
  entries.push_back(make_pair(80213, 1));
  AddList(groupOutput, LAVVIDEO_DITHERMODE, 80012, 0, LavSettings.video_dwDitherMode, entries, 80012);

  // DEINT HW/SW
  AddToggle(groupDeintHW, LAVVIDEO_HWDEINTMODE, 80006, 0, LavSettings.video_dwHWDeintMode);
  entries.clear();
  entries.push_back(make_pair(80210, 1));
  entries.push_back(make_pair(80211, 0));
  AddList(groupDeintHW, LAVVIDEO_HWDEINTOUT, 80007, 0, LavSettings.video_dwHWDeintOutput, entries, 80007);
  AddToggle(groupDeintHW, LAVVIDEO_HWDEINTHQ, 80008, 0, LavSettings.video_bHWDeintHQ);

  AddToggle(groupDeintSW, LAVVIDEO_SWDEINTMODE, 80011, 0, LavSettings.video_dwSWDeintMode);
  entries.clear();
  entries.push_back(make_pair(80210, 1));
  entries.push_back(make_pair(80211, 0));
  AddList(groupDeintSW, LAVVIDEO_SWDEINTOUT, 80007, 0, LavSettings.video_dwSWDeintOutput, entries, 80007);
}

void CGUIDialogLAVVideo::OnSettingChanged(const CSetting *setting)
{
  if (setting == NULL)
    return;

  CLavSettings &LavSettings = CMediaSettings::Get().GetCurrentLavSettings();

  CGUIDialogSettingsManualBase::OnSettingChanged(setting);
  const std::string &settingId = setting->GetId();

  if (settingId == LAVVIDEO_HWACCEL)
    LavSettings.video_dwHWAccel = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_NUMTHREADS)
    LavSettings.video_dwNumThreads = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_TRAYICON)
    LavSettings.video_bTrayIcon = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_STREAMAR)
    LavSettings.video_dwStreamAR = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_DEINTFILEDORDER)
    LavSettings.video_dwDeintFieldOrder = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_DEINTMODE)
    LavSettings.video_deintMode = (LAVDeintMode)static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_RGBRANGE)
    LavSettings.video_dwRGBRange = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_DITHERMODE)
    LavSettings.video_dwDitherMode = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_HWDEINTMODE)
    LavSettings.video_dwHWDeintMode = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_HWDEINTOUT)
    LavSettings.video_dwHWDeintOutput = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_HWDEINTHQ)
    LavSettings.video_bHWDeintHQ = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_SWDEINTMODE)
    LavSettings.video_dwSWDeintMode = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());
  if (settingId == LAVVIDEO_SWDEINTOUT)
    LavSettings.video_dwSWDeintMode = static_cast<int>(static_cast<const CSettingInt*>(setting)->GetValue());

  CGraphFilters::Get()->SaveLavSettings(LAVVIDEO);
  CGraphFilters::Get()->SaveLavSettings(LAVVIDEO);
}

void CGUIDialogLAVVideo::OnSettingAction(const CSetting *setting)
{
  if (setting == NULL)
    return;

  CGUIDialogSettingsManualBase::OnSettingAction(setting);
  const std::string &settingId = setting->GetId();

  if (settingId == LAVVIDEO_PROPERTYPAGE)
  {
    CGraphFilters::Get()->ShowLavFiltersPage(LAVVIDEO, true);
    this->Close();
  }
}


