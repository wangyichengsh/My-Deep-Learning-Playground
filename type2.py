# -*- coding: utf-8 -*-
"""
Created on Thu Jun  8 16:02:36 2017

@author: ASUS
"""
import numpy as np
from textblob import TextBlob
from nltk.corpus import stopwords
from gensim.models import Word2Vec
from keras.layers.core import Dense, Activation, Dropout
from keras.layers.recurrent import LSTM
from keras.layers.convolutional import Convolution1D,MaxPooling1D

from keras.models import Sequential
import time
import warnings

warnings.filterwarnings("ignore")

def load_data(X,Y,s=100,w=50,min_c =5,wind =5):
    reviews = open(X).readlines()
    labels =open(Y).readlines()
    y_row = []
    for label in labels:
        if(label =="positive\n"):
            y_row.append(np.array([1,0]))
        else:
            y_row.append(np.array([0,1]))

    word = []

    for line in reviews:
        textTB = TextBlob(line)
        word.append(textTB.words)
    wordRSW = [w for w in word if w.lower() not in stopwords.words('english')]
    mod = Word2Vec(wordRSW,window = wind,size =s,min_count=min_c)
    x_row =[]
    for i in range(len(reviews)):
        x = []
        b = 1
        for a in wordRSW[i]:
            if a in mod.wv.vocab:
                x.append(mod[a])
                b = b+1
            if b >w:
                break
        x = np.array(x)
        x = x.reshape(x.shape[0]*x.shape[1],1)
        if x.shape[0]<(s*w):
            t = int(s*w/x.shape[0])+1
            x = np.tile(x,t)
            x = x.reshape(x.shape[0]*x.shape[1],1)
            x = x[:s*w]
        x_row.append(x)
    x_row = np.array(x_row)
    x_row = x_row.reshape(x_row.shape[0],x_row.shape[1])
    y_row =np.array(y_row)
    row = round(0.7*x_row.shape[0])
    x_train = x_row[:row,:]
    y_train = y_row[:row,:]
    x_test = x_row[row:,:]
    y_test = y_row[row:,:]
    x_train = np.reshape(x_train, (x_train.shape[0], x_train.shape[1], 1))
    x_test = np.reshape(x_test, (x_test.shape[0], x_test.shape[1], 1))
    return[x_train,y_train,x_test,y_test]

def build_model(layers):  #layers [1,50,100,1]
    model = Sequential()
    
    model.add(Convolution1D(32,3,input_shape=(5000,1),subsample_length=2))
    model.add(Activation('relu'))
    
    model.add(MaxPooling1D(pool_length=2))
    
    #model.add(Convolution1D(64,3,subsample_length=2))
    #model.add(Activation('relu'))
    
    #model.add(MaxPooling1D(pool_length=2))
    
    model.add(LSTM(input_dim=layers[0],output_dim=layers[1],return_sequences=True))
    model.add(Dropout(0.5))

    model.add(LSTM(layers[2],return_sequences=False))
    model.add(Dropout(0.5))

    model.add(Dense(output_dim=layers[3]))
    model.add(Activation("softmax"))

    start = time.time()
    model.compile(loss="categorical_crossentropy", optimizer="rmsprop")

    print("Compilation Time : ", time.time() - start)
    return model

if __name__=='__main__':
    global_start_time = time.time()
    epochs  = 1

    x_train, y_train, x_test, y_test = load_data('reviews.txt','labels.txt')

    model = build_model([1, 100, 250, 2])

    model.fit(x_train,y_train,batch_size=150,nb_epoch=epochs,validation_split=0.05)

    predicted =model.predict(x_test)
