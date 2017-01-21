-- Cluster.hs
-- T. M. Kelley
-- Oct 28, 2016
-- (c) Copyright 2016 LANSLLC, all rights reserved

{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE TupleSections #-}
{-# LANGUAGE DeriveGeneric #-}
{-# LANGUAGE GeneralizedNewtypeDeriving #-}
{-# LANGUAGE TypeSynonymInstances #-}
{-# LANGUAGE FlexibleInstances #-}


-- Compute scores for different clusters
module Cluster where

import qualified Data.ByteString.Lazy as B
import qualified Data.HashSet as S
import qualified Data.HashMap.Strict as M
import qualified Data.Vector as V
import Data.Hashable (Hashable)
import GHC.Generics (Generic)
import Data.String (IsString)
import Data.Csv (ToField,FromField,ToRecord,FromRecord,decode,HasHeader(..))
import qualified Data.List as L
import qualified Data.Text as T
import Test.QuickCheck (elements,Gen,sample)
import Debug.Trace


type TStr = T.Text

newtype FuncName = FuncName TStr
  deriving (Eq,Ord,Show,Read,Hashable,IsString,Generic,ToField,FromField)
newtype FieldName = FieldName TStr
  deriving (Eq,Ord,Show,Read,Hashable,IsString,Generic,ToField,FromField)
data Use = Use FuncName FieldName deriving (Show,Read,Eq,Generic)

type Set = S.HashSet FieldName
type Map = M.HashMap FuncName Set
type Cluster = S.HashSet FieldName
type Score = Int
type FuncSet = S.HashSet FuncName
type FieldSet = S.HashSet FieldName
type Pairs = [(FieldName,FieldName)]
type FuncMap = M.HashMap FuncName Int
type FieldMap = M.HashMap FieldName Int

class Print a where
  printc :: a -> String

instance Print FuncName where
  printc (FuncName n) = T.unpack n
instance Print FieldName where
  printc (FieldName n) = T.unpack n
instance Print FieldSet where
  printc s = let strings = printc <$> S.toList s
    in L.intercalate "," strings

-- values for testing
m = read "fromList [(FuncName \"find_slopes\",fromList [FieldName \"mat\",FieldName \"sz_v\",FieldName \"sr_u\",FieldName \"num\",FieldName \"yp\",FieldName \"u\",FieldName \"volp\",FieldName \"sr_v\",FieldName \"xp\",FieldName \"sz_u\",FieldName \"v\"]),(FuncName \"top_right\",fromList [FieldName \"mat\",FieldName \"ycp\",FieldName \"xcp\"]),(FuncName \"compute_time\",fromList [FieldName \"u\",FieldName \"x\",FieldName \"cs\",FieldName \"v\",FieldName \"y\"]),(FuncName \"find_mp\",fromList [FieldName \"ar_v\",FieldName \"m\",FieldName \"rflux\",FieldName \"az_u\",FieldName \"az_v\",FieldName \"u\",FieldName \"mp\",FieldName \"zflux\",FieldName \"v\",FieldName \"ar_u\"]),(FuncName \"metrics\",fromList [FieldName \"xc\",FieldName \"yc\",FieldName \"x\"]),(FuncName \"energy\",fromList [FieldName \"ifburn\",FieldName \"vol\",FieldName \"mat\",FieldName \"p\",FieldName \"hs\",FieldName \"volp\",FieldName \"h\",FieldName \"DIV\"]),(FuncName \"hourglass\",fromList [FieldName \"qhy\",FieldName \"qhx\",FieldName \"u\",FieldName \"v\"]),(FuncName \"copy\",fromList [FieldName \"m\",FieldName \"mat\",FieldName \"qhy\",FieldName \"p\",FieldName \"syy\",FieldName \"num\",FieldName \"qhx\",FieldName \"qxx\",FieldName \"ie\",FieldName \"sxy\",FieldName \"r\",FieldName \"cs\",FieldName \"sxx\",FieldName \"stt\",FieldName \"qyy\"]),(FuncName \"bot_left\",fromList [FieldName \"mat\",FieldName \"ycp\",FieldName \"xcp\"]),(FuncName \"stress\",fromList [FieldName \"epsxy\",FieldName \"m\",FieldName \"mat\",FieldName \"hs\",FieldName \"epsyy\",FieldName \"qxx\",FieldName \"u\",FieldName \"volp\",FieldName \"h\",FieldName \"x\",FieldName \"cs\",FieldName \"DIV\",FieldName \"epsxx\",FieldName \"epstt\",FieldName \"v\",FieldName \"qyy\",FieldName \"y\"]),(FuncName \"boundary\",fromList [FieldName \"sz_v\",FieldName \"sr_u\",FieldName \"sr_v\",FieldName \"sz_u\"]),(FuncName \"find_center1\",fromList [FieldName \"m\",FieldName \"rflux\",FieldName \"az_u\",FieldName \"u\",FieldName \"mp\",FieldName \"zflux\",FieldName \"ar_u\"]),(FuncName \"rcenter1\",fromList [FieldName \"ar_v\",FieldName \"rflux\",FieldName \"Fr\",FieldName \"u\",FieldName \"v\",FieldName \"MFr\",FieldName \"ar_u\"]),(FuncName \"normal\",fromList [FieldName \"mat\",FieldName \"yp\",FieldName \"ycp\",FieldName \"u\",FieldName \"xp\",FieldName \"xcp\",FieldName \"v\"]),(FuncName \"zblock\",fromList [FieldName \"Fz\",FieldName \"sz_v\",FieldName \"xc\",FieldName \"sr_u\",FieldName \"yp\",FieldName \"az_u\",FieldName \"MFz\",FieldName \"yc\",FieldName \"az_v\",FieldName \"ycp\",FieldName \"u\",FieldName \"zflux\",FieldName \"sr_v\",FieldName \"xp\",FieldName \"sz_u\",FieldName \"xcp\",FieldName \"v\"]),(FuncName \"bot_right\",fromList [FieldName \"mat\",FieldName \"ycp\",FieldName \"xcp\"]),(FuncName \"displace_orig\",fromList [FieldName \"yp\",FieldName \"u\",FieldName \"x\",FieldName \"xp\",FieldName \"v\",FieldName \"y\"]),(FuncName \"metrics_block\",fromList [FieldName \"area\",FieldName \"vol\",FieldName \"xc\",FieldName \"yc\",FieldName \"x\",FieldName \"y\"]),(FuncName \"posboundary\",fromList [FieldName \"yp\",FieldName \"u\",FieldName \"x\",FieldName \"xp\",FieldName \"v\",FieldName \"y\"]),(FuncName \"flux\",fromList [FieldName \"Fz\",FieldName \"mat\",FieldName \"num\",FieldName \"yp\",FieldName \"MFz\",FieldName \"Fr\",FieldName \"ycp\",FieldName \"x\",FieldName \"xp\",FieldName \"right\",FieldName \"top\",FieldName \"xcp\",FieldName \"MFr\",FieldName \"y\"]),(FuncName \"shape\",fromList [FieldName \"vol\",FieldName \"mat\",FieldName \"x\",FieldName \"y\"]),(FuncName \"top_left\",fromList [FieldName \"mat\",FieldName \"ycp\",FieldName \"xcp\"]),(FuncName \"metricsp\",fromList [FieldName \"yp\",FieldName \"ycp\",FieldName \"xp\",FieldName \"xcp\"]),(FuncName \"uvflux_block\",fromList [FieldName \"area\",FieldName \"yp\",FieldName \"areap\",FieldName \"ycp\",FieldName \"volp\",FieldName \"xp\",FieldName \"xcp\"]),(FuncName \"relax\",fromList [FieldName \"x\",FieldName \"xpp\",FieldName \"ypp\",FieldName \"y\"]),(FuncName \"find_center3\",fromList [FieldName \"m\",FieldName \"rflux\",FieldName \"az_u\",FieldName \"u\",FieldName \"mp\",FieldName \"zflux\",FieldName \"ar_u\"]),(FuncName \"uvblock\",fromList [FieldName \"area\",FieldName \"m\",FieldName \"p\",FieldName \"syy\",FieldName \"qxx\",FieldName \"sxy\",FieldName \"r\",FieldName \"u\",FieldName \"x\",FieldName \"sxx\",FieldName \"stt\",FieldName \"v\",FieldName \"qyy\",FieldName \"y\"]),(FuncName \"smooth\",fromList [FieldName \"x\",FieldName \"xpp\",FieldName \"ypp\",FieldName \"y\"]),(FuncName \"copyp\",fromList [FieldName \"ycp\",FieldName \"xcp\"]),(FuncName \"zcenter1\",fromList [FieldName \"Fz\",FieldName \"az_u\",FieldName \"MFz\",FieldName \"az_v\",FieldName \"u\",FieldName \"zflux\",FieldName \"v\"]),(FuncName \"rblock\",fromList [FieldName \"ar_v\",FieldName \"rflux\",FieldName \"sz_v\",FieldName \"xc\",FieldName \"sr_u\",FieldName \"yp\",FieldName \"Fr\",FieldName \"yc\",FieldName \"ycp\",FieldName \"u\",FieldName \"sr_v\",FieldName \"xp\",FieldName \"sz_u\",FieldName \"xcp\",FieldName \"v\",FieldName \"MFr\",FieldName \"ar_u\"]),(FuncName \"rcenter2\",fromList [FieldName \"ar_v\",FieldName \"rflux\",FieldName \"Fr\",FieldName \"u\",FieldName \"v\",FieldName \"MFr\",FieldName \"ar_u\"]),(FuncName \"mesh\",fromList [FieldName \"yp\",FieldName \"u\",FieldName \"x\",FieldName \"xp\",FieldName \"v\",FieldName \"y\"]),(FuncName \"find_center2\",fromList [FieldName \"m\",FieldName \"rflux\",FieldName \"az_u\",FieldName \"u\",FieldName \"mp\",FieldName \"zflux\",FieldName \"ar_u\"]),(FuncName \"update\",fromList [FieldName \"vol\",FieldName \"mat\",FieldName \"right\",FieldName \"top\"]),(FuncName \"editor\",fromList [FieldName \"m\",FieldName \"mat\",FieldName \"p\",FieldName \"syy\",FieldName \"ie\",FieldName \"sxy\",FieldName \"r\",FieldName \"u\",FieldName \"mp\",FieldName \"x\",FieldName \"cs\",FieldName \"lt\",FieldName \"sxx\",FieldName \"stt\",FieldName \"v\",FieldName \"y\"]),(FuncName \"metricsp_block\",fromList [FieldName \"yp\",FieldName \"areap\",FieldName \"ycp\",FieldName \"volp\",FieldName \"xp\",FieldName \"xcp\"]),(FuncName \"properties\",fromList [FieldName \"ifburn\",FieldName \"vol\",FieldName \"m\",FieldName \"he_j\",FieldName \"mat\",FieldName \"p\",FieldName \"xc\",FieldName \"syy\",FieldName \"num\",FieldName \"ie\",FieldName \"yc\",FieldName \"sxy\",FieldName \"r\",FieldName \"cs\",FieldName \"lt\",FieldName \"sxx\",FieldName \"he_i\",FieldName \"stt\"])]"
flds = fields m
fncs = S.difference (funcs m) (S.fromList $ FuncName <$> ["find_center1","find_center2","find_center3","zcenter1","rcenter1","rcenter2"])
c1 = S.fromList (FieldName <$> ["sr_u","sr_v","sz_u","sz_v"])
c2 = S.fromList (FieldName <$> ["p","ie","r","sxx","sxy","syy","stt"])
c3 = S.fromList (FieldName <$> ["he_j","lt", "he_i"])
c4 = S.fromList (FieldName <$> ["m","mp"])
c5 = S.fromList (FieldName <$> ["qxx","qyy"])
c6 = S.fromList (FieldName <$> ["x","y"])
c7 = S.fromList (FieldName <$> ["az_u","az_v","zflux"])
c8 = S.fromList (FieldName <$> ["ar_u","ar_v","rflux"])
c9 = S.fromList (FieldName <$> ["epsxy","epsyy","epsxx","eps","epstt"])
c10 = S.fromList (FieldName <$> ["xc","yc"])
c11 = S.fromList (FieldName <$> ["xp","yp"])
c12 = S.fromList (FieldName <$> ["Fr","MFr"])
c13 = S.fromList (FieldName <$> ["Fz","MFz"])
c14 = S.fromList (FieldName <$> ["u","v"])
c15 = S.fromList (FieldName <$> ["hs","h","volp","DIV"])
c16 = S.fromList (FieldName <$> ["right","top"])
c17 = S.fromList (FieldName <$> ["xpp","ypp"])
c18 = S.fromList (FieldName <$> ["xcp","ycp"])
c19 = S.fromList (FieldName <$> ["qhx","qhy"])

flds2 = last $ scanl S.difference flds [c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14,c15,c16,c17,c18,c19]

mkFuncMap :: [FuncName] -> FuncMap
mkFuncMap fs = M.fromList $ zip fs [1..length fs]

mkFieldMap :: [FieldName] -> FieldMap
mkFieldMap fs = M.fromList $ zip fs [1..length fs]

mkPairs :: Map -> FuncMap -> FieldMap -> [(Int,Int)]
mkPairs m fncs flds = M.foldrWithKey gofunc [] m
  where gofunc f fldSet parz = case M.lookup f fncs of
          Just fncInt -> {- trace ("gofunc fncInt = " ++ show fncInt++ ", parz: " ++ show parz) $ -} S.foldr gofld parz fldSet
            where gofld fld parzz = case M.lookup fld flds of
                    Just fldInt -> {- trace ("\tAdding (" ++ show fncInt ++ "," ++ show fldInt ++ ")") $ -} (fldInt,fncInt) : parzz
                    Nothing -> parzz
          Nothing -> parz  -- If function is not in user's map, ignore it

printMM :: [(Int,Int)] -> String
printMM ps = foldr go "" ps
  where go (i1,i2) s = "{"++show i1++","++show i2++"},"++s

{- | Given a starting cluster, form the list of all
     clusters that add one field from the remaining set
     of felds; then score each cluster. Pick the first
     with the highest score; if it's greater than the initial
     score, then repeat with that cluster.

     Returns the best cluster (possibly the one you started with) and its score

     Assume that the input cluster is disjoint from the FieldSet.
-}
probeClustering :: Map -> FuncSet -> FieldSet -> Cluster -> Score -> (Cluster,Score)
probeClustering m fncs flds c s0 =
  let newcs = flip S.insert c <$> S.toList flds
      newScores = score m fncs <$> newcs
      (bestC,bestS) = L.maximumBy (\(c1,s1) (c2,s2) -> trace ("c1 compares: " ++ printc (c1) ++ "\t to c2: " ++ printc c2 ++ ", \ts1: " ++ show s1 ++ ",\t s2: " ++ show s2) $
        case s1>s2 of
          True -> GT
          False -> case s1 == s2 of
            True -> EQ
            False -> LT) $ (c,s0) : zip newcs newScores -- foldr go (c,s0) $ zip newcs newScores
  in if (bestC /= c)
     then trace ("bestC: " ++ printc bestC ++ " scored " ++ show bestS) $ probeClustering m fncs (S.difference flds bestC) bestC bestS
     else (c,s0)
  -- where go :: (Cluster,Score) -> (Cluster,Score) -> (Cluster,Score)
  --       go (c1,s1) (c2,s2) = trace ("c1 adds: " ++ printc (S.difference c1 c2) ++ "\t to c2: " ++ printc c2 ++ ", \ts1: " ++ show s1 ++ ",\t s2: " ++ show s2) $
  --         if (s1 > s2 ) then (c1,s1) else (c2,s2)

{- Given a cluster of fields, add one field at random and
  compute the change in score for the change in field. -}
varyCluster :: Map -> Cluster -> FuncSet -> FieldSet -> Gen Cluster
varyCluster m c fncs flds = do
  c' <- S.union c <$> S.singleton <$> pickField c flds
  let s0 = score m fncs c
      s1 = score m fncs c'
      diff = trace ("s1: " ++ show s1 ++ ", c': " ++ show c') $ s1 - s0
  return $ if diff > 0 then c' else c

pickField :: Cluster -> FieldSet -> Gen FieldName
pickField c flds = do
  let c' = S.difference flds c
  elements (S.toList c')

-- | Score a cluster of field names across a set of functions.
score :: Map -> FuncSet -> Cluster -> Score
score m fs c = S.foldr (scoreF m parz False) s0 fs
  where s0 = S.size c
        parz = pairs c

scores :: Map -> FuncSet -> Cluster -> [(FuncName,Score)]
scores m fs c =
  let scorz = fliplast scoreF m parz False s0 <$> flist
      flist = (S.toList fs)
  in zip flist scorz
  where s0 = S.size c
        parz = pairs c

fliplast f a b c d e = f a b c e d

scoreV :: Map -> FuncSet -> Cluster -> Score
scoreV m fs c = S.foldr (scoreF m parz True) s0 fs
  where s0 = S.size c
        parz = pairs c

-- | Compute score for a cluster, represented as pairs, for a single function.
-- For each pair in which both parts of the pair are used, one point is added,
-- for each pair in which one is used but not the other, one point
-- is subtracted, and no change is made is neither is used.
scoreF :: Map -> Pairs -> Bool -> FuncName -> Score -> Score
scoreF m pairs verbose f s0 = foldr go s0 pairs
  where go pair s = let p1 = uses m f (fst pair)
                        p2 = uses m f (snd pair)
                    in if verbose
                       then if trace ("func: "++ show f ++ "p1: " ++ show p1 ++ ", p2: " ++ show p2 ++ if (p1&&p2) then "\t+1" else if(p1/=p2) then "\t-1" else "\t0") (p1 && p2) then s + 1 else
                         if (p1 /= p2) then s - 1 else s
                       else if (p1 && p2) then s + 1 else
                         if (p1 /= p2) then s - 1 else s


-- | Does the function use the field?
uses :: Map -> FuncName -> FieldName -> Bool
uses m func fld = S.member fld s
  where s = M.lookupDefault S.empty func m

-- | Give the set of all pairs of fieldnames in a cluster
pairs :: Cluster ->  Pairs
pairs c = go [] $ S.toList c
  where go :: Pairs -> [FieldName] -> Pairs
        go ps [x] = ps
        go ps (n:ns) = go (ps ++ ((n,) <$>ns) ) ns


-- | Populate a function-uses map from CSV file
populateMap :: FilePath -> IO Map
populateMap f = do
  instr <- B.readFile f
  case decode NoHeader instr of
    Left msg -> error $ "Failed decoding: " ++ msg
    Right v -> return $ mkMap v

-- | Turn a vector of uses into a Map FuncName (Set FieldName)
-- This can probably be improved on.
mkMap :: V.Vector Use -> Map
mkMap v = let m0 = M.empty
  in V.foldr addEntry m0 v
  where
    addEntry :: Use -> Map -> Map
    addEntry (Use func fld) m = M.insertWith (S.union) func (S.singleton fld) m

-- | Get the functions used in a map
funcs :: Map -> FuncSet
funcs = S.fromList . M.keys
-- | Get the fields used
fields :: Map -> FieldSet
fields m = foldr S.union S.empty $ M.elems m


-- standalone instances
instance ToRecord FuncName
instance FromRecord FuncName
instance ToRecord FieldName
instance FromRecord FieldName
instance FromRecord Use
instance ToRecord Use

-- End of file
