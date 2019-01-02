
#include <algorithm>
#include "menu.hpp"
#include "gui/GameTDB.hpp"
#include "plugin/plugin.hpp"
#include "plugin/crc32.h"

wstringEx gameinfo_Synopsis_w;
wstringEx gameinfo_Title_w;

bool tdb_found = false;
u8 cnt_controlsreq = 0, cnt_controls = 0;
int synopsis_h;

void CMenu::_gameinfo(void)
{ 
	u8 page = 1;
	int pixels_to_skip = 10;	
	int amount_of_skips = 0;
	int xtra_skips = 0;
	int synopsis_th = 0;

	SetupInput();
	_showGameInfo();
	
	while(!m_exit)
	{
		_mainLoopCommon();
		if(BTN_HOME_PRESSED || BTN_B_PRESSED || !tdb_found)
			break;
		if((BTN_DOWN_PRESSED || BTN_DOWN_HELD) && page == 2 && synopsis_th > synopsis_h)
		{
			if((synopsis_th - amount_of_skips * pixels_to_skip) >= synopsis_h)
			{
				m_btnMgr.moveBy(m_gameinfoLblSynopsis, 0, -pixels_to_skip);
				amount_of_skips++;
			}
			else if((synopsis_th - amount_of_skips * pixels_to_skip) < synopsis_h && xtra_skips == 0)
			{
				xtra_skips = pixels_to_skip - ((synopsis_th - amount_of_skips * pixels_to_skip) - synopsis_h);
				m_btnMgr.moveBy(m_gameinfoLblSynopsis, 0, -xtra_skips);
			}
		}
		else if((BTN_UP_PRESSED || BTN_UP_HELD) && page == 2)
		{
			if(xtra_skips > 0)
			{
				m_btnMgr.moveBy(m_gameinfoLblSynopsis, 0, xtra_skips);
				xtra_skips = 0;
			}
			else if (amount_of_skips > 0)
			{
				m_btnMgr.moveBy(m_gameinfoLblSynopsis, 0, pixels_to_skip);
				amount_of_skips--;
			}
		}
		else if(BTN_RIGHT_PRESSED && page == 1 && !gameinfo_Synopsis_w.empty())
		{
			page = 2;
			amount_of_skips = 0;

			m_btnMgr.hide(m_gameinfoLblID, true);
			m_btnMgr.hide(m_gameinfoLblDev, true);
			m_btnMgr.hide(m_gameinfoLblRegion, true);
			m_btnMgr.hide(m_gameinfoLblPublisher, true);
			m_btnMgr.hide(m_gameinfoLblRlsdate, true);
			m_btnMgr.hide(m_gameinfoLblGenre, true);
			m_btnMgr.hide(m_gameinfoLblRating, true);
			m_btnMgr.hide(m_gameinfoLblWifiplayers, true);
			m_btnMgr.hide(m_gameinfoLblSnap, true);
			m_btnMgr.hide(m_gameinfoLblCartDisk, true);
			m_btnMgr.hide(m_gameinfoLblOverlay, true);

			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControlsReq); ++i)
				if(m_gameinfoLblControlsReq[i] != -1)
					m_btnMgr.hide(m_gameinfoLblControlsReq[i], true);

			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControls); ++i)
				if(m_gameinfoLblControls[i] != -1)
					m_btnMgr.hide(m_gameinfoLblControls[i], true);

			// When showing synopsis, only show user labels 2 and 3
			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblUser); ++i)
				if(i < ARRAY_SIZE(m_gameinfoLblUser) / 2)
					m_btnMgr.hide(m_gameinfoLblUser[i], true);
				else
					m_btnMgr.show(m_gameinfoLblUser[i]);

			m_btnMgr.reset(m_gameinfoLblSynopsis);
			m_btnMgr.show(m_gameinfoLblSynopsis, false);
			m_btnMgr.getTotalHeight(m_gameinfoLblSynopsis, synopsis_th);
			m_btnMgr.moveBy(m_gameinfoLblSynopsis, 0, -1);
		}
		else if(BTN_LEFT_PRESSED)
		{
			page = 1;
			m_btnMgr.show(m_gameinfoLblID);
			m_btnMgr.show(m_gameinfoLblDev);
			m_btnMgr.show(m_gameinfoLblRegion);	
			m_btnMgr.show(m_gameinfoLblPublisher);
			m_btnMgr.show(m_gameinfoLblRlsdate);
			m_btnMgr.show(m_gameinfoLblGenre);
			m_btnMgr.show(m_gameinfoLblRating);
			m_btnMgr.show(m_gameinfoLblWifiplayers);
			m_btnMgr.show(m_gameinfoLblSnap);
			m_btnMgr.show(m_gameinfoLblCartDisk);
			m_btnMgr.show(m_gameinfoLblOverlay);

			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControlsReq); ++i)
				if(m_gameinfoLblControlsReq[i] != -1 && i < cnt_controlsreq)
					m_btnMgr.show(m_gameinfoLblControlsReq[i]);

			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControls); ++i)
				if(m_gameinfoLblControls[i] != -1 && i < cnt_controls)
					m_btnMgr.show(m_gameinfoLblControls[i]);

			// When showing synopsis, only show user labels 2 and 3
			for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblUser); ++i)
				if(i < ARRAY_SIZE(m_gameinfoLblUser) / 2)
					m_btnMgr.show(m_gameinfoLblUser[i]);
				else
					m_btnMgr.hide(m_gameinfoLblUser[i], true);

			m_btnMgr.hide(m_gameinfoLblSynopsis,true);
		}
	}
	_hideGameInfo(false);
}

void CMenu::_hideGameInfo(bool instant)
{
	m_btnMgr.hide(m_gameinfoLblID, instant);
	m_btnMgr.hide(m_gameinfoLblTitle, instant);
	m_btnMgr.hide(m_gameinfoLblSynopsis, instant);
	m_btnMgr.hide(m_gameinfoLblDev, instant);
	m_btnMgr.hide(m_gameinfoLblRegion, instant);
	m_btnMgr.hide(m_gameinfoLblPublisher, instant);
	m_btnMgr.hide(m_gameinfoLblRlsdate, instant);
	m_btnMgr.hide(m_gameinfoLblGenre, instant);
	m_btnMgr.hide(m_gameinfoLblRating, instant);
	m_btnMgr.hide(m_gameinfoLblWifiplayers, instant);
	m_btnMgr.hide(m_gameinfoLblSnap, instant);
	m_btnMgr.hide(m_gameinfoLblCartDisk, instant);
	m_btnMgr.hide(m_gameinfoLblOverlay, instant);
	
	for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControlsReq); ++i)
		if(m_gameinfoLblControlsReq[i] != -1)
			m_btnMgr.hide(m_gameinfoLblControlsReq[i], instant);

	for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblUser); ++i)
		m_btnMgr.hide(m_gameinfoLblUser[i], instant);

	for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControls); ++i)
		if(m_gameinfoLblControls[i] != -1)
			m_btnMgr.hide(m_gameinfoLblControls[i], instant);
}

void CMenu::_showGameInfo(void)
{
	_setBg(m_gameinfoBg, m_gameinfoBg);

	_textGameInfo();
	
	if(tdb_found)
	{
		m_btnMgr.show(m_gameinfoLblID);
		m_btnMgr.show(m_gameinfoLblTitle);
		m_btnMgr.show(m_gameinfoLblRating);
		m_btnMgr.show(m_gameinfoLblRegion);	
		m_btnMgr.show(m_gameinfoLblDev);
		m_btnMgr.show(m_gameinfoLblPublisher);
		m_btnMgr.show(m_gameinfoLblRlsdate);
		m_btnMgr.show(m_gameinfoLblGenre);
		m_btnMgr.show(m_gameinfoLblWifiplayers);

		if(m_current_view == COVERFLOW_PLUGIN)
		{
			m_btnMgr.show(m_gameinfoLblSnap);
			m_btnMgr.show(m_gameinfoLblCartDisk);
			m_btnMgr.show(m_gameinfoLblOverlay);
		}

		for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblUser); ++i)
			if(i < ARRAY_SIZE(m_gameinfoLblUser) / 2)
				m_btnMgr.show(m_gameinfoLblUser[i]);
		
		for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControlsReq); ++i)
			if(m_gameinfoLblControlsReq[i] != -1 && i < cnt_controlsreq)
				m_btnMgr.show(m_gameinfoLblControlsReq[i]);
			
		for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControls); ++i)
			if(m_gameinfoLblControls[i] != -1 && i < cnt_controls)
				m_btnMgr.show(m_gameinfoLblControls[i]);
	}
	else
		error(_t("errgame18", L"No game info!"));
}

void CMenu::_initGameInfoMenu()
{
	TexData emptyTex;
	_addUserLabels(m_gameinfoLblUser, 0, 1, "GAMEINFO");
	_addUserLabels(m_gameinfoLblUser, 2, 1, "GAMEINFO");
	
	m_gameinfoBg = _texture("GAMEINFO/BG", "texture", theme.bg, false);
	m_gameinfoLblID = _addLabel("GAMEINFO/GAMEID", theme.txtFont, L"", 40, 110, 420, 75, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblGenre = _addLabel("GAMEINFO/GENRE", theme.txtFont, L"", 40, 140, 460, 56, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblDev = _addLabel("GAMEINFO/DEVELOPER", theme.txtFont, L"", 40, 170, 460, 56, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblPublisher = _addLabel("GAMEINFO/PUBLISHER", theme.txtFont, L"", 40, 200, 460, 56, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblRlsdate = _addLabel("GAMEINFO/RLSDATE", theme.txtFont, L"", 40, 230, 460, 56, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblRegion = _addLabel("GAMEINFO/REGION", theme.txtFont, L"", 40, 260, 460, 56, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblRating = _addLabel("GAMEINFO/RATING", theme.txtFont, L"", 550, 380, 48, 60, theme.txtFontColor, 0, m_rating);
	m_gameinfoLblSynopsis = _addLabel("GAMEINFO/SYNOPSIS", theme.txtFont, L"", 40, 80, 560, 280, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP);
	m_gameinfoLblWifiplayers = _addLabel("GAMEINFO/WIFIPLAYERS", theme.txtFont, L"", 550, 110, 68, 60, theme.txtFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_TOP,m_wifi);
	m_gameinfoLblSnap = _addLabel("GAMEINFO/SNAP", theme.titleFont, L"", 485, 200, m_snap.width, m_snap.height, theme.titleFontColor, 0, m_snap);
	m_gameinfoLblOverlay = _addLabel("GAMEINFO/OVERLAY", theme.txtFont, L"",  485, 200, m_snap.width, m_snap.height, theme.titleFontColor, 0, m_overlay);
	m_gameinfoLblCartDisk = _addLabel("GAMEINFO/CART", theme.txtFont, L"", 435, 364, m_cart.width, m_cart.height, theme.txtFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE,m_cart);

	_addUserLabels(m_gameinfoLblUser, 1, 1, "GAMEINFO");
	_addUserLabels(m_gameinfoLblUser, 3, 2, "GAMEINFO");

	m_gameinfoLblTitle = _addLabel("GAMEINFO/TITLE", theme.titleFont, L"", 0, 10, 640, 60, theme.titleFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);

	for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControlsReq); ++i)
	{
		string dom(fmt("GAMEINFO/CONTROLSREQ%i", i + 1));
		m_gameinfoLblControlsReq[i] = _addLabel(dom.c_str(), theme.txtFont, L"", 40 + (i*60), 310, 60, 40, theme.txtFontColor, 0, emptyTex);
		_setHideAnim(m_gameinfoLblControlsReq[i], dom.c_str(), 0, -100, 0.f, 0.f);
	}

	for(u8 i = 0; i < ARRAY_SIZE(m_gameinfoLblControls); ++i)
	{
		string dom(fmt("GAMEINFO/CONTROLS%i", i + 1));
		m_gameinfoLblControls[i] = _addLabel(dom.c_str(), theme.txtFont, L"", 40 + (i*60), 380, 60, 40, theme.txtFontColor, 0, emptyTex);
		_setHideAnim(m_gameinfoLblControls[i], dom.c_str(), 0, -100, 0.f, 0.f);
	}
	// 
	_setHideAnim(m_gameinfoLblID, "GAMEINFO/GAMEID",0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblTitle, "GAMEINFO/TITLE", 0, 0, -2.f, 0.f);
	_setHideAnim(m_gameinfoLblRating, "GAMEINFO/RATING", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblSynopsis, "GAMEINFO/SYNOPSIS", 0, 700, 1.f, 1.f);
	_setHideAnim(m_gameinfoLblRegion, "GAMEINFO/REGION", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblDev, "GAMEINFO/DEVELOPER", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblPublisher, "GAMEINFO/PUBLISHER", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblRlsdate, "GAMEINFO/RLSDATE", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblGenre, "GAMEINFO/GENRE", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblWifiplayers, "GAMEINFO/WIFIPLAYERS", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblSnap, "GAMEINFO/SNAP", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblCartDisk, "GAMEINFO/CART", 0, -100, 0.f, 0.f);
	_setHideAnim(m_gameinfoLblOverlay, "GAMEINFO/OVERLAY", 0, -100, 0.f, 0.f);
	// 
	_hideGameInfo(true);
	synopsis_h = m_theme.getInt("GAMEINFO/SYNOPSIS", "height", 280);
}

void CMenu::_textGameInfo(void)
{
	cnt_controlsreq = 0;
	cnt_controls = 0;

	GameTDB gametdb;
	char platformName[264];

	if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
	{
		// Check the platform name corresponding to the current magic number.
		// We can't use magic # directly since it'd require hardcoding values and a # can be several systems(genplus) 
		// We can't rely on coverfolder either. Different systems can share the same folder. Or combined plugins used for the same system.
		Config m_platform;
		m_platform.unload();
		m_platform.load(fmt("%s/platform.ini", m_pluginDataDir.c_str()) );
		snprintf(platformName, sizeof(platformName), "%s", m_platform.getString("PLUGINS", m_plugin.PluginMagicWord).c_str());
		m_platform.unload();
		gametdb.OpenFile(fmt("%s/%s/%s.xml", m_pluginDataDir.c_str(), platformName, platformName));
	}
	else
	{
		gametdb.OpenFile(fmt("%s/wiitdb.xml", m_settingsDir.c_str()));
	}

	gametdb.SetLanguageCode(m_loc.getString(m_curLanguage, "gametdb_code", "EN").c_str());
	const char *TMP_Char = NULL;
	tdb_found = gametdb.IsLoaded();
	if(tdb_found)
	{
		char GameID[7];
		GameID[6] = '\0';

		string ShortName;

		// Clear text and textures 
		TexData emptyTex;
		gameinfo_Synopsis_w.fromUTF8("");
		m_btnMgr.setText(m_gameinfoLblDev, wfmt(_fmt("",L"")), true);
		m_btnMgr.setText(m_gameinfoLblPublisher, wfmt(_fmt("",L"")), true);
		m_btnMgr.setText(m_gameinfoLblRegion, wfmt(_fmt("",L"")), true);
		m_btnMgr.setText(m_gameinfoLblRating, wfmt(_fmt("",L"")), true);
		m_btnMgr.setTexture(m_gameinfoLblRating, emptyTex);

		m_btnMgr.setTexture(m_gameinfoLblSnap, emptyTex);
		m_btnMgr.setTexture(m_gameinfoLblCartDisk, emptyTex);
		m_btnMgr.setTexture(m_gameinfoLblOverlay, emptyTex);

		// Get Game ID
		if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
		{
			const dir_discHdr *GameHdr = CoverFlow.getHdr();
			ShortName = m_plugin.GetRomName(GameHdr);
			strncpy(GameID, m_plugin.GetRomId(GameHdr, m_pluginDataDir.c_str(), platformName, ShortName).c_str(), 6);
		}
		else
		{
			strncpy(GameID, CoverFlow.getId(), 6);
		}

		if(gametdb.GetTitle(GameID, TMP_Char))
		{
			gameinfo_Title_w.fromUTF8(TMP_Char);
			m_btnMgr.setText(m_gameinfoLblTitle, gameinfo_Title_w);

			if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
			{
				// Try to find images by game's name
				if(gametdb.GetName(GameID, TMP_Char))
				{
					const char *snap_path = NULL;
					const char *cart_path = NULL;
					const char *overlay_path = NULL;

					// Use real filename without extension for arcade games.
					if(strcasestr(platformName, "ARCADE") || strcasestr(platformName, "CPS") || !strncasecmp(platformName, "NEOGEO", 6))
					{
						snap_path = fmt("%s/%s/%s.png", m_snapDir.c_str(), platformName, ShortName.c_str());
						cart_path = fmt("%s/%s/%s_2D.png", m_cartDir.c_str(), platformName, ShortName.c_str());
					}
					// Name from the database.
					else
					{
						snap_path = fmt("%s/%s/%s.png", m_snapDir.c_str(), platformName, TMP_Char);
						cart_path = fmt("%s/%s/%s_2D.png", m_cartDir.c_str(), platformName, TMP_Char);
					}

					// Try to find images by game's ID
					if(!fsop_FileExist( snap_path ))
					{
						snap_path = fmt("%s/%s/%s.png", m_snapDir.c_str(), platformName, GameID);
						cart_path = fmt("%s/%s/%s_2D.png", m_cartDir.c_str(), platformName, GameID);

						if(!fsop_FileExist( snap_path ))
						{
							TexHandle.Cleanup(m_snap);
							TexHandle.Cleanup(m_cart);
						}
					}

					TexHandle.fromImageFile(m_snap, snap_path);
					m_btnMgr.setTexture(m_gameinfoLblSnap, m_snap, m_snap.width, m_snap.height);
					TexHandle.fromImageFile(m_cart, cart_path);

					if( m_cart.height > 112 )
					{
						m_btnMgr.setTexture(m_gameinfoLblCartDisk, m_cart, 114, 128);
					}
					else
					{
						m_btnMgr.setTexture(m_gameinfoLblCartDisk, m_cart, 160, 112);
					}

					overlay_path = fmt("%s/%s_overlay.png", m_snapDir.c_str(), platformName);

					if(fsop_FileExist( overlay_path ))
					{
						TexHandle.fromImageFile(m_overlay, overlay_path);
						m_btnMgr.setTexture(m_gameinfoLblOverlay, m_overlay, m_overlay.width, m_overlay.height);
					}
					else
					{
						m_btnMgr.setTexture(m_gameinfoLblOverlay, emptyTex);
					}			
				}
			}
		}
		else
		{
			tdb_found = false;
			gametdb.CloseFile();
			return;
		}

		if(gametdb.GetSynopsis(GameID, TMP_Char))
		{						
			gameinfo_Synopsis_w.fromUTF8(TMP_Char);
			m_btnMgr.setText(m_gameinfoLblSynopsis, gameinfo_Synopsis_w);
		}
		m_btnMgr.setText(m_gameinfoLblID, wfmt(L"GameID: %s", GameID), true);

		// Only show retrieved data else the text is often underneath the snapshot.
		// It's still too long sometimes. Maybe, we should cut the string at a max length.
		if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
		{
			if(gametdb.GetDeveloper(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblDev, wfmt(_fmt("",L"%s"), TMP_Char), true);
			if(gametdb.GetPublisher(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblPublisher, wfmt(_fmt("",L"%s"), TMP_Char), true);
			if(gametdb.GetRegion(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblRegion, wfmt(_fmt("gameinfo3",L"Region: %s"), TMP_Char), true);
			if(gametdb.GetGenres(GameID, TMP_Char))
			{
				m_btnMgr.setText(m_gameinfoLblGenre, wfmt(_fmt("",L"%s"), TMP_Char), true);
			}
			else
			{
				// Genre not found, show nothing.
				m_btnMgr.setText(m_gameinfoLblGenre, wfmt(_fmt("",L"")), true);
			}
		}
		else
		{
			if(gametdb.GetDeveloper(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblDev, wfmt(_fmt("gameinfo1",L"Developer: %s"), TMP_Char), true);
			if(gametdb.GetPublisher(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblPublisher, wfmt(_fmt("gameinfo2",L"Publisher: %s"), TMP_Char), true);
			if(gametdb.GetRegion(GameID, TMP_Char))
				m_btnMgr.setText(m_gameinfoLblRegion, wfmt(_fmt("gameinfo3",L"Region: %s"), TMP_Char), true);
			if(gametdb.GetGenres(GameID, TMP_Char))
			{
				vector<string> genres = stringToVector(TMP_Char, ',');
				string s;
				for(u32 i = 0; i < genres.size(); ++i)
				{
					if(i > 0)
						s.append(", ");// add comma & space between genres
					s.append(genres[i]);
				}
				m_btnMgr.setText(m_gameinfoLblGenre, wfmt(_fmt("gameinfo5",L"Genre: %s"), s.c_str()), true);
			}
		}

		int PublishDate = gametdb.GetPublishDate(GameID);
		int year = PublishDate >> 16;
		int day = PublishDate & 0xFF;
		int month = (PublishDate >> 8) & 0xFF;
		switch(CONF_GetRegion())
		{
			case 0:
			case 4:
			case 5:
				if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("",L"%i-%i-%i"), year, month, day), true);
				else
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("gameinfo4",L"Release Date: %i-%i-%i"), year, month, day), true);
				break;
			case 1:
				if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("",L"%i-%i-%i"), month, day, year), true);
				else
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("gameinfo4",L"Release Date: %i-%i-%i"), month, day, year), true);
				break;
			case 2:
				if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("",L"%i-%i-%i"), day, month, year), true);
				else
					m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("gameinfo4",L"Release Date: %i-%i-%i"), day, month, year), true);
				break;
		}

		// Only display year or nothing if there's no date at all
		if(day == 0 && month == 0)
		{
			if(year != 0)
				m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("",L"%i"), year), true);
			else
				m_btnMgr.setText(m_gameinfoLblRlsdate, wfmt(_fmt("",L"")), true);
		}

		//Ratings
		TexHandle.fromImageFile(m_rating, fmt("%s/norating.png", m_imgsDir.c_str()));
		const char *RatingValue = NULL;
		if(gametdb.GetRatingValue(GameID, RatingValue))
		{
			switch(gametdb.GetRating(GameID))
			{
				case GAMETDB_RATING_TYPE_CERO:
					if(RatingValue[0] == 'A')
						TexHandle.fromImageFile(m_rating, fmt("%s/cero_a.png", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'B')
						TexHandle.fromImageFile(m_rating, fmt("%s/cero_b.png", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'D')
						TexHandle.fromImageFile(m_rating, fmt("%s/cero_d.png", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'C')
						TexHandle.fromImageFile(m_rating, fmt("%s/cero_c.png", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'Z')
						TexHandle.fromImageFile(m_rating, fmt("%s/cero_z.png", m_imgsDir.c_str()));
					break;
				case GAMETDB_RATING_TYPE_ESRB:
					if(RatingValue[0] == 'E')
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_e.jpg", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "EC", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_ec.jpg", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "E10+", 4) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_eten.jpg", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'T')
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_t.jpg", m_imgsDir.c_str()));
					else if(RatingValue[0] == 'M')
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_m.jpg", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "AO", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/esrb_ao.jpg", m_imgsDir.c_str()));
					break;
				case GAMETDB_RATING_TYPE_PEGI:
					if(RatingValue[0] == '3')
						TexHandle.fromImageFile(m_rating, fmt("%s/pegi_3.png", m_imgsDir.c_str()));
					else if(RatingValue[0] == '7')
						TexHandle.fromImageFile(m_rating, fmt("%s/pegi_7.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "12", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/pegi_12.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "16", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/pegi_16.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "18", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/pegi_18.png", m_imgsDir.c_str()));
					break;
				case GAMETDB_RATING_TYPE_GRB:
					if(RatingValue[0] == 'A')
						TexHandle.fromImageFile(m_rating, fmt("%s/grb_a.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "12", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/grb_12.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "15", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/grb_15.png", m_imgsDir.c_str()));
					else if(memcmp(RatingValue, "18", 2) == 0)
						TexHandle.fromImageFile(m_rating, fmt("%s/grb_18.png", m_imgsDir.c_str()));
					break;
				default:
					break;
			}
		}
		// Display the user's mark /20 instead because most of the rating data is missing.
		if(CoverFlow.getHdr()->type == TYPE_PLUGIN)
		{ 
			if(gametdb.GetRatingValue(GameID, RatingValue))
			{
				if(RatingValue[0] != '\0')
				{
					m_btnMgr.setText(m_gameinfoLblRating, wfmt(_fmt("",L"%s/20"), RatingValue), true);
				}
			}
			else
			{
				m_btnMgr.setText(m_gameinfoLblRating, wfmt(_fmt("",L"")), true);
			}
		}
		else
		{
			m_btnMgr.setText(m_gameinfoLblRating, wfmt(_fmt("",L"")), true);
			m_btnMgr.setTexture(m_gameinfoLblRating, m_rating);
		}
		//Wifi players
		int WifiPlayers = gametdb.GetWifiPlayers(GameID);
		if(WifiPlayers == 1)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi1.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 2)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi2.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 4)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi4.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 8)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi8.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 10)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi10.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 12)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi12.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 16)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi16.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 18)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi18.png", m_imgsDir.c_str()));
		else if(WifiPlayers == 32)
			TexHandle.fromImageFile(m_wifi, fmt("%s/wifi32.png", m_imgsDir.c_str()));
		if(WifiPlayers > 0)
			m_btnMgr.setTexture(m_gameinfoLblWifiplayers, m_wifi);
		else 
			m_btnMgr.setTexture(m_gameinfoLblWifiplayers, emptyTex);

		//check required controlls
		bool wiimote = false;
		bool nunchuk = false;
		bool classiccontroller = false;
		bool balanceboard = false;
		bool dancepad = false;
		bool guitar = false;
		bool gamecube = false;
		bool motionplus = false;
		bool drums = false;
		bool microphone = false;
		bool wheel = false;
		bool keyboard = false;
		bool udraw = false;
		bool zapper = false;

		vector<Accessory> Accessories;
		gametdb.GetAccessories(GameID, Accessories);
		for(vector<Accessory>::iterator acc_itr = Accessories.begin(); acc_itr != Accessories.end(); acc_itr++)
		{
			if(!acc_itr->Required)
				continue;
			if(strcmp((acc_itr->Name).c_str(), "wiimote") == 0)
				wiimote = true;
			else if(strcmp((acc_itr->Name).c_str(), "nunchuk") == 0)
				nunchuk = true;
			else if(strcmp((acc_itr->Name).c_str(), "guitar") == 0)
				guitar = true;
			else if(strcmp((acc_itr->Name).c_str(), "drums") == 0)
				drums = true;
			else if(strcmp((acc_itr->Name).c_str(), "dancepad") == 0)
				dancepad = true;
			else if(strcmp((acc_itr->Name).c_str(), "motionplus") == 0)
				motionplus = true;
			else if(strcmp((acc_itr->Name).c_str(), "microphone") == 0)
				microphone = true;
			else if(strcmp((acc_itr->Name).c_str(), "balanceboard") == 0)
				balanceboard = true;
			else if(strcmp((acc_itr->Name).c_str(), "udraw") == 0)
				udraw = true;
		}
		u8 x = 0;
		u8 max_controlsReq = ARRAY_SIZE(m_gameinfoLblControlsReq);
		
		if(wiimote && x < max_controlsReq)
		{
			u8 players = gametdb.GetPlayers(GameID);
			if(players >= 10)
				players /= 10;
			if(players == 1)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote1.png", m_imgsDir.c_str()));
			else if(players == 2)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote2.png", m_imgsDir.c_str()));
			else if(players == 3)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote3.png", m_imgsDir.c_str()));
			else if(players == 4)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote4.png", m_imgsDir.c_str()));
			else if(players == 5)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote5.png", m_imgsDir.c_str()));
			else if(players == 6)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote6.png", m_imgsDir.c_str()));
			else if(players == 8)
				TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/wiimote8.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 20, 60);
			x++;
		}
		if(nunchuk && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/nunchukR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(guitar && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/guitarR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(drums && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/drumsR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(motionplus && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/motionplusR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 20, 60);
			x++;
		}
		if(dancepad && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/dancepadR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(microphone && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/microphoneR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(balanceboard && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/balanceboardR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		if(udraw && x < max_controlsReq)
		{
			TexHandle.fromImageFile(m_controlsreq[x], fmt("%s/udrawR.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControlsReq[x] ,m_controlsreq[x], 52, 60);
			x++;
		}
		cnt_controlsreq = x;

		//check optional controlls
		wiimote = false;
		nunchuk = false;
		classiccontroller = false;
		balanceboard = false;
		dancepad = false;
		guitar = false;
		gamecube = false;
		motionplus = false;
		drums = false;
		microphone = false;
		wheel = false;
		keyboard = false;
		udraw = false;
		zapper = false;
		for(vector<Accessory>::iterator acc_itr = Accessories.begin(); acc_itr != Accessories.end(); acc_itr++)
		{
			if(acc_itr->Required)
				continue;
			if(strcmp((acc_itr->Name).c_str(), "classiccontroller") == 0)
				classiccontroller = true;
			else if(strcmp((acc_itr->Name).c_str(), "nunchuk") == 0)
				nunchuk = true;
			else if(strcmp((acc_itr->Name).c_str(), "guitar") == 0)
				guitar = true;
			else if(strcmp((acc_itr->Name).c_str(), "drums") == 0)
				drums = true;
			else if(strcmp((acc_itr->Name).c_str(), "dancepad") == 0)
				dancepad = true;
			else if(strcmp((acc_itr->Name).c_str(), "motionplus") == 0)
				motionplus = true;
			else if(strcmp((acc_itr->Name).c_str(), "balanceboard") == 0)
				balanceboard = true;
			else if(strcmp((acc_itr->Name).c_str(), "microphone") == 0)
				microphone = true;
			else if(strcmp((acc_itr->Name).c_str(), "gamecube") == 0)
				gamecube = true;
			else if(strcmp((acc_itr->Name).c_str(), "keyboard") == 0)
				keyboard = true;
			else if(strcmp((acc_itr->Name).c_str(), "zapper") == 0)
				zapper = true;
			else if(strcmp((acc_itr->Name).c_str(), "wheel") == 0)
				wheel = true;
			else if(strcmp((acc_itr->Name).c_str(), "udraw") == 0)
				udraw = true;
		}
		x = 0;
		u8 max_controls = ARRAY_SIZE(m_gameinfoLblControls);
		if(classiccontroller && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/classiccontroller.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(nunchuk && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/nunchuk.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(guitar && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/guitar.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(drums && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/drums.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(dancepad && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/dancepad.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(motionplus && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/motionplus.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 20, 60);
			x++;
		}
		if(balanceboard && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/balanceboard.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(microphone && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/microphone.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 48, 60);
			x++;
		}
		if(gamecube && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/gcncontroller.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 48, 60);
			x++;
		}
		if(keyboard && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/keyboard.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(udraw && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/udraw.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		if(zapper && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/zapper.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 70);
			x++;
		}
		if(wheel && x < max_controls)
		{
			TexHandle.fromImageFile(m_controls[x], fmt("%s/wheel.png", m_imgsDir.c_str()));
			m_btnMgr.setTexture(m_gameinfoLblControls[x] ,m_controls[x], 52, 60);
			x++;
		}
		cnt_controls = x;
	}
	else
		m_btnMgr.setText(m_gameinfoLblTitle, wfmt(_fmt("gameinfo6",L"No Gameinfo"), true));

	gametdb.CloseFile();
}
