
DROP TABLE IF EXISTS `playercreateinfo_bars`;
CREATE TABLE IF NOT EXISTS `playercreateinfo_bars` (
  `race` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `button` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `action` mediumint(10) unsigned DEFAULT NULL,
  `type` tinyint(3) unsigned DEFAULT NULL,
  `misc` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`race`,`class`,`button`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COMMENT='Player System';

REPLACE INTO `playercreateinfo_bars` (`race`, `class`, `button`, `action`, `type`, `misc`) VALUES
	(1, 1, 72, 88163, 0, 0),
	(1, 1, 73, 88161, 0, 0),
	(1, 1, 81, 59752, 0, 0),
	(1, 1, 84, 6603, 0, 0),
	(1, 1, 96, 6603, 0, 0),
	(1, 1, 108, 6603, 0, 0),
	(1, 2, 0, 35395, 0, 0),
	(1, 2, 9, 59752, 0, 0),
	(1, 3, 0, 3044, 0, 0),
	(1, 3, 2, 59752, 0, 0),
	(1, 3, 9, 9, 48, 0),
	(1, 3, 8, 982, 0, 0),
	(1, 4, 0, 1752, 0, 0),
	(1, 4, 9, 59752, 0, 0),
	(1, 5, 0, 585, 0, 0),
	(1, 5, 9, 59752, 0, 0),
	(1, 8, 0, 133, 0, 0),
	(1, 8, 9, 59752, 0, 0),
	(1, 9, 0, 686, 0, 0),
	(1, 9, 9, 59752, 0, 0),
	(1, 9, 10, 10, 48, 0),
	(2, 1, 72, 88163, 0, 0),
	(2, 1, 73, 88161, 0, 0),
	(2, 1, 81, 20572, 0, 0),
	(2, 1, 84, 6603, 0, 0),
	(2, 1, 96, 6603, 0, 0),
	(2, 1, 108, 6603, 0, 0),
	(2, 3, 0, 3044, 0, 0),
	(2, 3, 2, 20572, 0, 0),
	(2, 3, 9, 9, 48, 0),
	(2, 3, 8, 982, 0, 0),
	(2, 4, 0, 1752, 0, 0),
	(2, 4, 9, 20572, 0, 0),
	(2, 7, 0, 403, 0, 0),
	(2, 7, 9, 20572, 0, 0),
	(2, 7, 72, 403, 0, 0),
	(2, 8, 0, 133, 0, 0),
	(2, 8, 9, 33702, 0, 0),
	(2, 9, 0, 686, 0, 0),
	(2, 9, 9, 33702, 0, 0),
	(2, 9, 10, 10, 48, 0),
	(3, 1, 72, 88163, 0, 0),
	(3, 1, 73, 88161, 0, 0),
	(3, 1, 81, 20594, 0, 0),
	(3, 1, 84, 6603, 0, 0),
	(3, 1, 96, 6603, 0, 0),
	(3, 1, 108, 6603, 0, 0),
	(3, 2, 0, 35395, 0, 0),
	(3, 2, 9, 20594, 0, 0),
	(3, 3, 0, 3044, 0, 0),
	(4, 3, 1, 75, 0, 0),
	(3, 3, 9, 9, 48, 0),
	(3, 3, 8, 982, 0, 0),
	(3, 3, 2, 20594, 0, 0),
	(3, 4, 0, 1752, 0, 0),
	(3, 4, 9, 20594, 0, 0),
	(3, 5, 0, 585, 0, 0),
	(3, 5, 9, 20594, 0, 0),
	(3, 7, 0, 6603, 0, 0),
	(3, 7, 1, 403, 0, 0),
	(3, 7, 9, 20594, 0, 0),
	(3, 7, 72, 403, 0, 0),
	(3, 8, 0, 133, 0, 0),
	(3, 8, 9, 20594, 0, 0),
	(3, 9, 0, 686, 0, 0),
	(3, 9, 9, 20594, 0, 0),
	(3, 9, 10, 10, 48, 0),
	(4, 1, 72, 88163, 0, 0),
	(4, 1, 73, 88161, 0, 0),
	(4, 1, 81, 58984, 0, 0),
	(4, 1, 84, 6603, 0, 0),
	(4, 1, 96, 6603, 0, 0),
	(4, 1, 108, 6603, 0, 0),
	(4, 3, 0, 3044, 0, 0),
	(3, 3, 1, 75, 0, 0),
	(4, 3, 9, 9, 48, 0),
	(4, 3, 8, 982, 0, 0),
	(4, 3, 2, 58984, 0, 0),
	(4, 4, 0, 1752, 0, 0),
	(4, 4, 9, 58984, 0, 0),
	(4, 4, 81, 58984, 0, 0),
	(4, 5, 0, 585, 0, 0),
	(4, 5, 9, 58984, 0, 0),
	(4, 5, 81, 58984, 0, 0),
	(4, 8, 0, 133, 0, 0),
	(4, 8, 9, 58984, 0, 0),
	(4, 11, 0, 5176, 0, 0),
	(4, 11, 9, 58984, 0, 0),
	(4, 11, 72, 6603, 0, 0),
	(4, 11, 74, 58984, 0, 0),
	(4, 11, 84, 6603, 0, 0),
	(4, 11, 96, 6603, 0, 0),
	(4, 11, 108, 6603, 0, 0),
	(5, 1, 72, 88163, 0, 0),
	(5, 1, 73, 88161, 0, 0),
	(5, 1, 81, 20577, 0, 0),
	(5, 1, 84, 6603, 0, 0),
	(5, 1, 96, 6603, 0, 0),
	(5, 1, 108, 6603, 0, 0),
	(5, 3, 0, 3044, 0, 0),
	(5, 3, 2, 20577, 0, 0),
	(5, 3, 9, 9, 48, 0),
	(5, 3, 8, 982, 0, 0),
	(5, 4, 0, 1752, 0, 0),
	(5, 4, 9, 20577, 0, 0),
	(5, 5, 0, 585, 0, 0),
	(5, 5, 9, 20577, 0, 0),
	(5, 8, 0, 133, 0, 0),
	(5, 8, 9, 20577, 0, 0),
	(5, 9, 0, 686, 0, 0),
	(5, 9, 9, 20577, 0, 0),
	(5, 9, 10, 10, 48, 0),
	(6, 1, 72, 88163, 0, 0),
	(6, 1, 73, 88161, 0, 0),
	(6, 1, 81, 20549, 0, 0),
	(6, 1, 84, 6603, 0, 0),
	(6, 1, 96, 6603, 0, 0),
	(6, 1, 108, 6603, 0, 0),
	(6, 2, 0, 35395, 0, 0),
	(6, 2, 9, 20549, 0, 0),
	(6, 3, 0, 3044, 0, 0),
	(2, 3, 1, 75, 0, 0),
	(6, 3, 9, 9, 48, 0),
	(6, 3, 8, 982, 0, 0),
	(6, 3, 2, 20549, 0, 0),
	(6, 5, 0, 585, 0, 0),
	(6, 5, 9, 20549, 0, 0),
	(6, 7, 0, 403, 0, 0),
	(6, 7, 9, 20549, 0, 0),
	(6, 7, 75, 20549, 0, 0),
	(6, 11, 0, 403, 0, 0),
	(6, 11, 9, 20549, 0, 0),
	(6, 11, 72, 6603, 0, 0),
	(6, 11, 75, 20549, 0, 0),
	(6, 11, 84, 6603, 0, 0),
	(6, 11, 96, 6603, 0, 0),
	(6, 11, 108, 6603, 0, 0),
	(7, 1, 72, 88163, 0, 0),
	(7, 1, 73, 88161, 0, 0),
	(7, 1, 84, 6603, 0, 0),
	(7, 1, 96, 6603, 0, 0),
	(7, 1, 108, 6603, 0, 0),
	(7, 4, 0, 1752, 0, 0),
	(7, 5, 0, 585, 0, 0),
	(7, 8, 0, 133, 0, 0),
	(7, 9, 0, 686, 0, 0),
	(7, 9, 10, 10, 48, 0),
	(8, 1, 72, 88163, 0, 0),
	(8, 1, 73, 88161, 0, 0),
	(8, 1, 81, 26297, 0, 0),
	(8, 1, 84, 6603, 0, 0),
	(8, 1, 96, 6603, 0, 0),
	(8, 1, 108, 6603, 0, 0),
	(8, 3, 0, 3044, 0, 0),
	(8, 3, 2, 26297, 0, 0),
	(8, 3, 9, 9, 48, 0),
	(8, 3, 8, 982, 0, 0),
	(8, 4, 0, 1752, 0, 0),
	(8, 4, 9, 26297, 0, 0),
	(8, 4, 76, 26297, 0, 0),
	(8, 5, 0, 585, 0, 0),
	(8, 5, 9, 26297, 0, 0),
	(8, 7, 0, 403, 0, 0),
	(8, 7, 9, 26297, 0, 0),
	(8, 7, 72, 403, 0, 0),
	(8, 8, 0, 133, 0, 0),
	(8, 8, 9, 26297, 0, 0),
	(8, 9, 0, 686, 0, 0),
	(8, 9, 9, 26297, 0, 0),
	(8, 9, 10, 10, 48, 0),
	(8, 11, 0, 5176, 0, 0),
	(8, 11, 9, 26297, 0, 0),
	(8, 11, 72, 6603, 0, 0),
	(8, 11, 84, 6603, 0, 0),
	(8, 11, 96, 6603, 0, 0),
	(9, 1, 72, 88163, 0, 0),
	(9, 1, 73, 88161, 0, 0),
	(9, 1, 81, 69070, 0, 0),
	(9, 1, 82, 69041, 0, 0),
	(9, 3, 0, 3044, 0, 0),
	(9, 3, 8, 982, 0, 0),
	(9, 3, 3, 69070, 0, 0),
	(9, 3, 2, 69041, 0, 0),
	(9, 3, 9, 9, 48, 0),
	(9, 4, 0, 1752, 0, 0),
	(9, 4, 9, 69070, 0, 0),
	(9, 4, 10, 69041, 0, 0),
	(9, 5, 0, 585, 0, 0),
	(9, 5, 9, 69070, 0, 0),
	(9, 5, 10, 69041, 0, 0),
	(9, 7, 0, 403, 0, 0),
	(9, 7, 9, 69070, 0, 0),
	(9, 7, 10, 69041, 0, 0),
	(9, 7, 72, 403, 0, 0),
	(9, 8, 0, 133, 0, 0),
	(9, 8, 9, 69070, 0, 0),
	(9, 8, 10, 69041, 0, 0),
	(9, 9, 0, 686, 0, 0),
	(9, 9, 9, 69070, 0, 0),
	(9, 9, 10, 69041, 0, 0),
	(9, 9, 11, 10, 48, 0),
	(10, 1, 72, 88163, 0, 0),
	(10, 1, 73, 88161, 0, 0),
	(10, 1, 81, 69179, 0, 0),
	(10, 2, 0, 35395, 0, 0),
	(10, 2, 9, 28730, 0, 0),
	(10, 3, 0, 3044, 0, 0),
	(10, 3, 2, 80483, 0, 0),
	(10, 3, 9, 9, 48, 0),
	(10, 3, 8, 982, 0, 0),
	(10, 4, 0, 1752, 0, 0),
	(10, 4, 9, 25046, 0, 0),
	(10, 5, 0, 585, 0, 0),
	(10, 5, 9, 28730, 0, 0),
	(10, 8, 0, 133, 0, 0),
	(10, 8, 9, 28730, 0, 0),
	(10, 9, 0, 686, 0, 0),
	(10, 9, 9, 28730, 0, 0),
	(10, 9, 10, 10, 48, 0),
	(11, 1, 72, 88163, 0, 0),
	(11, 1, 73, 88161, 0, 0),
	(11, 1, 81, 28880, 0, 0),
	(11, 1, 84, 6603, 0, 0),
	(11, 1, 96, 6603, 0, 0),
	(11, 1, 108, 6603, 0, 0),
	(11, 2, 0, 35395, 0, 0),
	(11, 2, 9, 59542, 0, 0),
	(11, 2, 83, 4540, 128, 0),
	(11, 3, 0, 3044, 0, 0),
	(11, 3, 2, 59543, 0, 0),
	(11, 3, 9, 9, 48, 0),
	(11, 3, 8, 982, 0, 0),
	(1, 3, 1, 75, 0, 0),
	(11, 3, 1, 75, 0, 0),
	(22, 3, 1, 75, 0, 0),
	(10, 3, 1, 75, 0, 0),
	(11, 5, 0, 585, 0, 0),
	(11, 5, 9, 59544, 0, 0),
	(11, 5, 83, 4540, 128, 0),
	(11, 7, 0, 403, 0, 0),
	(11, 7, 9, 59547, 0, 0),
	(11, 8, 0, 133, 0, 0),
	(11, 8, 9, 59548, 0, 0),
	(11, 8, 83, 4540, 128, 0),
	(22, 1, 72, 88163, 0, 0),
	(22, 1, 73, 88161, 0, 0),
	(22, 3, 0, 3044, 0, 0),
	(22, 3, 9, 9, 48, 0),
	(22, 3, 8, 982, 0, 0),
	(22, 4, 0, 1752, 0, 0),
	(22, 5, 0, 585, 0, 0),
	(22, 8, 0, 133, 0, 0),
	(22, 9, 0, 686, 0, 0),
	(22, 9, 10, 10, 48, 0),
	(22, 11, 0, 5176, 0, 0),
	(22, 11, 72, 6603, 0, 0),
	(22, 11, 84, 6603, 0, 0),
	(22, 11, 96, 6603, 0, 0),
	(1, 6, 0, 6603, 0, 0),
	(1, 6, 1, 49576, 0, 0),
	(1, 6, 2, 45477, 0, 0),
	(1, 6, 3, 45462, 0, 0),
	(1, 6, 4, 45902, 0, 0),
	(1, 6, 5, 47541, 0, 0),
	(1, 6, 11, 59752, 0, 0),
	(2, 6, 0, 6603, 0, 0),
	(2, 6, 1, 49576, 0, 0),
	(2, 6, 2, 45477, 0, 0),
	(2, 6, 3, 45462, 0, 0),
	(2, 6, 4, 45902, 0, 0),
	(2, 6, 5, 47541, 0, 0),
	(2, 6, 10, 20572, 0, 0),
	(3, 6, 0, 6603, 0, 0),
	(3, 6, 1, 49576, 0, 0),
	(3, 6, 2, 45477, 0, 0),
	(3, 6, 3, 45462, 0, 0),
	(3, 6, 4, 45902, 0, 0),
	(3, 6, 5, 47541, 0, 0),
	(4, 6, 0, 6603, 0, 0),
	(4, 6, 1, 49576, 0, 0),
	(4, 6, 2, 45477, 0, 0),
	(4, 6, 3, 45462, 0, 0),
	(4, 6, 4, 45902, 0, 0),
	(4, 6, 5, 47541, 0, 0),
	(4, 6, 10, 58984, 0, 0),
	(4, 6, 83, 58984, 0, 0),
	(5, 6, 0, 6603, 0, 0),
	(5, 6, 1, 49576, 0, 0),
	(5, 6, 2, 45477, 0, 0),
	(5, 6, 3, 45462, 0, 0),
	(5, 6, 4, 45902, 0, 0),
	(5, 6, 5, 47541, 0, 0),
	(5, 6, 10, 20577, 0, 0),
	(6, 6, 0, 6603, 0, 0),
	(6, 6, 1, 49576, 0, 0),
	(6, 6, 2, 45477, 0, 0),
	(6, 6, 3, 45462, 0, 0),
	(6, 6, 4, 45902, 0, 0),
	(6, 6, 5, 47541, 0, 0),
	(6, 6, 10, 20549, 0, 0),
	(6, 6, 75, 20549, 0, 0),
	(7, 6, 0, 6603, 0, 0),
	(7, 6, 1, 49576, 0, 0),
	(7, 6, 2, 45477, 0, 0),
	(7, 6, 3, 45462, 0, 0),
	(7, 6, 4, 45902, 0, 0),
	(7, 6, 5, 47541, 0, 0),
	(7, 6, 10, 20589, 0, 0),
	(7, 6, 72, 6603, 0, 0),
	(7, 6, 83, 117, 128, 0),
	(7, 6, 84, 6603, 0, 0),
	(7, 6, 96, 6603, 0, 0),
	(7, 6, 108, 6603, 0, 0),
	(8, 6, 0, 6603, 0, 0),
	(8, 6, 1, 49576, 0, 0),
	(8, 6, 2, 45477, 0, 0),
	(8, 6, 3, 45462, 0, 0),
	(8, 6, 4, 45902, 0, 0),
	(8, 6, 5, 47541, 0, 0),
	(8, 6, 10, 26297, 0, 0),
	(9, 6, 0, 6603, 0, 0),
	(9, 6, 1, 49576, 0, 0),
	(9, 6, 2, 45477, 0, 0),
	(9, 6, 3, 45462, 0, 0),
	(9, 6, 4, 45902, 0, 0),
	(9, 6, 5, 47541, 0, 0),
	(9, 6, 9, 69070, 0, 0),
	(9, 6, 10, 69041, 0, 0),
	(9, 6, 11, 69046, 0, 0),
	(10, 6, 0, 6603, 0, 0),
	(10, 6, 1, 49576, 0, 0),
	(10, 6, 2, 45477, 0, 0),
	(10, 6, 3, 45462, 0, 0),
	(10, 6, 4, 45902, 0, 0),
	(10, 6, 5, 47541, 0, 0),
	(10, 6, 6, 50613, 0, 0),
	(11, 6, 0, 6603, 0, 0),
	(11, 6, 1, 49576, 0, 0),
	(11, 6, 2, 45477, 0, 0),
	(11, 6, 3, 45462, 0, 0),
	(11, 6, 4, 45902, 0, 0),
	(11, 6, 5, 47541, 0, 0),
	(11, 6, 10, 59545, 0, 0),
	(22, 6, 0, 6603, 0, 0),
	(22, 6, 1, 49576, 0, 0),
	(22, 6, 2, 45477, 0, 0),
	(22, 6, 3, 45462, 0, 0),
	(22, 6, 4, 45902, 0, 0),
	(22, 6, 5, 47541, 0, 0),
	(22, 6, 9, 68992, 0, 0),
	(22, 6, 10, 68996, 0, 0),
	(22, 6, 11, 87840, 0, 0),
	(5, 3, 1, 75, 0, 0),
	(6, 3, 1, 75, 0, 0),
	(8, 3, 1, 75, 0, 0),
	(9, 3, 1, 75, 0, 0);

	UPDATE `world_db_version` SET `LastUpdate` = '2015-11-20_02_playercreateinfo_bars' WHERE `LastUpdate` = '2015-11-20_01_playercreateinfo';