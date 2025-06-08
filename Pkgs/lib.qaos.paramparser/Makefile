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

Module = lib.qaos.paramparser

Srcs = \
	Dev/ParamParser.cpp

Dirs = \
	../lib.qaos.basis.dev/Inc \
	../lib.qaos.paramparser.dev/Inc


Incs = $(foreach dir,$(Dirs),-I$(dir))

Deps = $(foreach dir,$(Dirs),$(wildcard $(dir)/*.hpp))

Objs = $(addprefix $(Tmp)/, $(basename $(notdir $(Srcs))))
Objs:= $(addsuffix .bc, $(Objs))




# Targets

Build: Basis $(Out)/Package.moq


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Tmp)
	@mkdir -p $(Lib)


Clean:
	@rm -rf $(Out)



# Files

$(Out)/Package.moq: $(Lib)/libParamParser.o $(Lib)/libParamParser.so
	@echo "  - 📦️ Packing"

	@rm -f $(Out)/Package.moq
	@tar -cf $(Out)/Package.moq  -C @Out Lib



$(Lib)/libParamParser.so: $(Lib)/libParamParser.o.bc

	@echo "  - 🔨 libParamParser.so"

	@$(CP) -shared $(Lib)/libParamParser.o.bc \
		-o $(Lib)/libParamParser.so


$(Lib)/libParamParser.o: $(Lib)/libParamParser.o.bc

	@echo "  - 🔨 libParamParser.o"

	@$(CP) -c $(Lib)/libParamParser.o.bc \
		-o $(Lib)/libParamParser.o



$(Lib)/libParamParser.o.bc: $(Objs)
	@echo "  - 🔗 libParamParser.o.bc"
	@llvm-link $(Objs) \
		-o $(Lib)/libParamParser.o.bc



$(Tmp)/%.bc: */%.cpp $(Deps)
	@echo "  - ⚙️ $(notdir $@)"
	@$(LP) -DModule=\"$(Module)\" $(Incs) $< \
		-o $@

