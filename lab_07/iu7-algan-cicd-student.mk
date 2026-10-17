# # Обязательно в реализации
# # - Цели НЕ менять
# # - Зависимости и сценарии поменять на необходимые

# # Рекомендуется проект выстраивать вокруг собственных сценариев сборки
# # Рекомендуется в этом файле оставить только свои вызовы "make build",
# # "cmake .", "msBuild proj1.sln ./out" и операции копирования

# # Сценарий интерфейса специально не носит название "makefile" -
# # Вы можете отсюда обращаться к своим сценариям

# # Цели и зависимости в этом и в своих makefile и его аналогах остаются
# # под ответственностью студента.

# # Никто специально не будет искать и ругаться на Вас, если у Вас,
# # например, перекомпилируется весь проект при изменении одного модуля
# # (преступное неиспользование возможностей инкрементальной сборки).
# # Но если, всё-таки, преподаватели заметят грязь - то спросят.

# # Собрать на сервере И положить на проверку pdf отчёта
# # Допускается использование libreoffice, latex, xelatex
# # UPD 2025-2026: по настоянию лектора с этого года только latex
# ready/report.pdf: ./report/report.pdf
# 	mkdir -p ./ready
# 	cp ./report/report.pdf ready/report.pdf


# report/report.pdf:
# 	cd report && \
# 	pdflatex -interaction=batchmode "report.tex" && \
#     pdflatex -interaction=batchmode "report.tex" && \
# 	gs -sDEVICE=pdfwrite \
# 		-dCompatibilityLevel=1.4 \
#         -dNOPAUSE \
#         -dOptimize=true \
#         -dQUIET \
#         -dBATCH \
#         -dRemoveUnusedFonts=true \
#         -dRemoveUnusedImages=true \
#         -dOptimizeResources=true \
#         -dDetectDuplicateImages \
#         -dCompressFonts=true \
#         -dEmbedAllFonts=true \
#         -dSubsetFonts=true \
#         -dPreserveAnnots=true \
#         -dPreserveMarkedContent=true \
#         -dPreserveOverprintSettings=true \
#         -dPreserveHalftoneInfo=true \
#         -dPreserveOPIComments=true \
#         -dPreserveDeviceN=true \
#         -dMaxInlineImageSize=0 \
#         -sOutputFile="report_compressed.pdf" \
#         "report.pdf" && \
# 	mv report_compressed.pdf report.pdf



# ready/stud-unit-test-report.json:
# 	mkdir -p ready
# 	cd code && make coverage
# 	cp code/tests/stud-unit-test-report-prev.json ready/stud-unit-test-report.json


# ready/app-cli-debug:
# 	mkdir -p ready
# 	cd code && make debug
# 	cp code/prog_debug ready/app-cli-debug


# ready/app-cli-release:
# 	mkdir -p ready
# 	cd code && make release
# 	cp code/prog ready/app-cli-release

# # Очистка
# .PHONY: clean
# clean:
# 	cd code && make clean
# 	rm -rf ready
# 	echo OK

# # --

# # Если хочется добавить в образ на сервере что-то,
# # можно обратиться к @alexodnodvorcev прямо в комментарии к MR.

# # --