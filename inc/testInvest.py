import pandas as pd
import random
from pandas import read_csv

if __name__ == '__main__':

  import argparse
  parser = argparse.ArgumentParser()
  parser.add_argument("-f","--file",type=str)
  parser.add_argument("-m_CropProduction",action="store_true")
  parser.add_argument("-m_CropPollination",action="store_true")
  parser.add_argument("-m_Erosion",action="store_true")
  parser.add_argument("-f_CropProduction",action="store_true")
  parser.add_argument("-f_CropPollination",action="store_true")
  parser.add_argument("-f_Erosion",action="store_true")

  args = parser.parse_args()
  toRead = args.file+'\\toInvest.csv'

  inputs  = pd.read_csv(toRead, delimiter=',')
  print(inputs)

  cropProd = random.uniform(-0.001,+0.0001)
  erosion  = random.uniform(-0.001,+0.0001)

  with open(str(args.file+'\\toManage.csv'), 'w',newline='') as csvFile:
    csvFile.write('"reg","cropProd","erosion"\n')
    csvFile.write('"tza",%f,%f\n' % (cropProd,erosion))
    csvFile.close()
