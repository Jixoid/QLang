#
#  This file is part of QLang.
#
#  This file is licensed under the GNU Lesser General Public License version 3 (LGPLv3).
#
#  You should have received a copy of GNU Lesser General Public License
#  along with QLang. If not, see <https://www.gnu.org/licenses/>.
#
#  Copyright (c) 2025 by Kadir Aydın.
#




include Make.inc



Build:
	@echo "♦️ Build"
	@echo


	@$(MAKE) -C Pkgs  Build
	@echo

	@echo "Successfuly Compiled!"
	@echo


Clean:
	@echo "♦️ Clean"
	@echo


	@$(MAKE) -C Pkgs  Clean
	@echo

