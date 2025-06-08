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




include ../../Make.inc


# Config

Module = com.jixoid.qlang

Srcs = \
	Dev/Main.cpp \
	Dev/Parser.cpp \
	Dev/Proc.cpp \
	Dev/Ver.cpp

Dirs = \
	../lib.qaos.basis.dev/Inc \
	../lib.qaos.paramparser.dev/Inc

Links = \
	../lib.qaos.paramparser/$(Lib)/libParamParser.o


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))




# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Bin)
	

Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: $(Bin)/Main.elf
	@echo "  - 📦️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Bin


$(Bin)/Main.elf: $(Bin)/Main.elf.bc
	@echo "  - 🔨 Main.elf"
	@$(CP) $(Links) $(Bin)/Main.elf.bc \
		-o $(Bin)/Main.elf


$(Bin)/Main.elf.bc: $(Objs) ../lib.qaos.paramparser/$(Lib)/libParamParser.o
	@echo "  - 🔗 Main.elf.bc"
	@llvm-link $(Objs) \
		-o $(Bin)/Main.elf.bc


$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

