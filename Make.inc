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




# Colors

GREEN = \e[1;32m
BLUE = \e[1;34m
ORANGE = \e[1;33m
RED = \e[1;91m
RESET = \e[0m

# Params

Out = @Out
Tmp = $(Out)/Tmp
Bin = $(Out)/Bin
Lib = $(Out)/Lib
Nuc = $(Out)/Nuc


# Compile

_O_Level = -O2

CArg = -fPIC \
	-Wno-unused-result \
  -Wno-unused-command-line-argument \


LC = clang   -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) $(CONFIG) -std=c20
LP = clang++ -emit-llvm -c $(_O_Level) $(CArg) $(CONFIG_CC_Params) $(CONFIG) -std=c++20

CC = clang   $(_O_Level) $(CArg)
CP = clang++ $(_O_Level) $(CArg)

