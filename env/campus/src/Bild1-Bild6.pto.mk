# makefile for panorama stitching, created by hugin 

# the output panorama
TARGET_PREFIX=Bild1-Bild6
TARGET=Bild1-Bild6.tif
PROJECT_FILE=F:\source-wip\edu2\edu2\id1\env\campus\src\Bild1-Bild6.pto

# Input images
INPUT_IMAGES=F:\source-wip\edu2\edu2\id1\env\campus\src\Bild1.jpg F:\source-wip\edu2\edu2\id1\env\campus\src\Bild2.jpg F:\source-wip\edu2\edu2\id1\env\campus\src\Bild3.jpg F:\source-wip\edu2\edu2\id1\env\campus\src\Bild4.jpg F:\source-wip\edu2\edu2\id1\env\campus\src\Bild5.jpg F:\source-wip\edu2\edu2\id1\env\campus\src\Bild6.jpg 

# remapped images
REMAPPED_IMAGES=Bild1-Bild60000.tif Bild1-Bild60001.tif Bild1-Bild60002.tif Bild1-Bild60003.tif Bild1-Bild60004.tif Bild1-Bild60005.tif 

# Tool configuration
NONA=D:\Programme\hugin-0.7_beta4_windows/xrc/..\nona.exe
PTSTITCHER=PTStitcher
PTMENDER=D:\Programme\hugin-0.7_beta4_windows/xrc/..\PTmender.exe
PTBLENDER=D:\Programme\hugin-0.7_beta4_windows/xrc/..\PTblender.exe
PTMASKER=D:\Programme\hugin-0.7_beta4_windows/xrc/..\PTblender.exe
PTROLLER=D:\Programme\hugin-0.7_beta4_windows/xrc/..\PTroller.exe
ENBLEND=D:\Programme\hugin-0.7_beta4_windows/xrc/..\enblend.exe
SMARTBLEND=smartblend.exe

# options for the programs
ENBLEND_OPTS=-v  -f1024x1024


# including template D:\Programme\hugin-0.7_beta4_windows/xrc/data/nona_enblend.mk
# Rules for generating the pano

all: $(TARGET)

clean: $(REMAPPED_IMAGES)
	$(RM) $(REMAPPED_IMAGES)

$(REMAPPED_IMAGES): $(INPUT_IMAGES)
	$(NONA) -o $(TARGET_PREFIX) $(PROJECT_FILE)

$(TARGET): $(REMAPPED_IMAGES)
	$(RM) $(TARGET)
	$(ENBLEND) $(ENBLEND_OPTS) -o $(TARGET) $(REMAPPED_IMAGES)


