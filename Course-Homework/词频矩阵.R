setwd("D:/study/深度学习/期末项目")
library(tm)
l=read.table("labels.txt",header = FALSE)
r=read.csv("reviews.txt",header = FALSE)
r.res <- r[r!=" "] 
r.stop=removeWords(r.res,stopwords("english"))
seg = function(x){
  y = unlist(strsplit(x, "\\W"))
  y = y[y != ""]
  return(y)
}
words = lapply(r.stop,FUN=seg)
txt = Corpus(x = VectorSource(words))
dtm = DocumentTermMatrix(txt,control = list(wordLengths = c(1,Inf)))
d=removeSparseTerms(x=dtm,sparse =0.9)
df=as.matrix(d)
dd=as.data.frame(df)
l$V1=as.factor(l$V1)
data=dd
data$y=l$V1
ind<-sample(2,nrow(data),replace = TRUE,prob=c(0.7,0.3))
data1=data[ind==1,]
data2=data[ind==2,]

glm1=glm(data1$y~.,family=binomial(link=logit),data=data1)
p=predict(glm1,data2)
p_log=exp(p)/(1+exp(p))
caculate_acc = function(threshold,  pred_prob, 
                         true_class = data2$y) {
  
  pred_class = ifelse(pred_prob > threshold, 1, 0)
  t = table(as.factor(pred_class), true_class)
  
  TP = t[2, 2]
  FP = t[2, 1]
  TT = t[1, 1]
  FF = t[1, 2]
  acc = (TT + TP)/(TP+FP+TT+FF)
  acc
}

thresh = seq(0.4, 0.6, by = 0.01)
acc = rep(0, length(thresh))

for (i in seq_along(thresh)) {
  acc[i] = caculate_acc(threshold = thresh[i], pred_prob = p_log)
}
plot(thresh, acc, type = "l", xlab = "threshold")


#words = unlist(strsplit(r.stop, "\\W"))
#words = words[words != ""]
#c=tail(sort(table(tolower(words))),200)
#c=as.data.frame(c)
#c=c[which(c$Freq<40000),]
#letterCloud(c,word="烦",size=2)
#wordcloud2(c,shape="cardioid",size =0.3)